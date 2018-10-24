
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "common.h"

/* void parse_vertex( */


enum ply_state {
    PLY_MAGIC,
    PLY_HEADER,
    PLY_VERTICES,
    PLY_INDICES
};

static void skip_line(const char **cursor) {
    while (*((*cursor)++) != '\n')
        ;
}

static int consume_string(const char **cursor, const char *str) {
    int len = strlen(str);

    if (memcmp(*cursor, str, len) == 0)
        *cursor += len;
    else
        return 0;

    return 1;
}

static int parse_element(const char **cursor, int *nverts) {
    int ok;
    ok = consume_string(cursor, "element vertex ");
    if (!ok)
        return 0;

    *nverts = atoi(*cursor);

    consume_string(cursor, "\n");
    return *nverts != 0;
}


static int parse_vertex(const char **cursor, float *v, float *n, u8 *c) {
    int matched =
        sscanf(*cursor, "%f %f %f %f %f %f %hhu %hhu %hhu",
            &v[0], &v[1], &v[2],
            &n[0], &n[1], &n[2],
            &c[0], &c[1], &c[2]);
    skip_line(cursor);

    return matched == 9;
}


static int parse_header(const char **cursor, int *nverts) {
    int res = parse_element(cursor, nverts);
    if (res)
        return 1;

    skip_line(cursor);
    return 0;
}

static int parse_magic(const char **cursor) {
    return consume_string(cursor, "ply\n");
}

int parse_ply(const char *filename) {
    size_t len;
    int nverts = 0;
    int success = 0;
    int done = 0;
    int res = 0;
    enum ply_state state = PLY_MAGIC;
    const char *data = file_contents(filename, &len);
    const char *p = data;

    float vert[3], norm[3];
    u8 color[3];
 
    while(!done) {
        switch (state) {
        case PLY_MAGIC:
            res = parse_magic(&p);
            if (!res) {
                printf("failed to parse ply magic header\n");
                break;
            }
            state = PLY_HEADER;
            break;
        case PLY_HEADER:
            res = parse_header(&p, &nverts);
            if (res)
                printf("got nverts %d\n", nverts);
            if (consume_string(&p, "end_header\n"))
                state = PLY_VERTICES;
            break;
        case PLY_VERTICES:
            res = parse_vertex(&p, vert, norm, color);
            if (!res) {
                done = 1;
                break;
            }
            printf("got vert %f %f %f %f %f %f %hhu %hhu %hhu\n",
                   vert[0], vert[1], vert[2],
                   norm[0], norm[1], norm[2],
                   color[0], color[1], color[2]);
            break;
        case PLY_INDICES:
            done = 1;
            break;
        }
    }

    free((void*)data);

    return success;
}
