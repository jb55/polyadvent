
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "common.h"
#include "ply.h"

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

static int parse_element(const char **cursor, const char *element, int *nverts) {
    int ok;
    ok = consume_string(cursor, "element ");
    if (!ok)
        return 0;

    ok  = consume_string(cursor, element);
    ok |= consume_string(cursor, " ");

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


static int parse_indices(const char **cursor, int *inds) {
    // NOTE: only support tris inds for now
    int matched =
        sscanf(*cursor, "3 %d %d %d", &inds[0], &inds[1], &inds[2]);

    skip_line(cursor);

    return matched == 3;
}

static int parse_header(const char **cursor, int *nverts, int *ninds) {
    int ok = 0;
    ok |= parse_element(cursor, "vertex", nverts);
    ok |= parse_element(cursor, "faces", ninds);

    if (!ok)
        return 0;

    skip_line(cursor);
    return 0;
}

static int parse_magic(const char **cursor) {
    return consume_string(cursor, "ply\n");
}


int parse_ply(const char *filename, void *context, vertex_cb *vert_cb,
              index_cb *ind_cb) {
    size_t len;
    int nverts = 0;
    int ninds = 0;
    int success = 0;
    int done = 0;
    int res = 0;

    enum ply_state state = PLY_MAGIC;
    const char *data = file_contents(filename, &len);
    const char *p = data;

    float vert[3], norm[3];
    int inds[3];
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
            res = parse_header(&p, &nverts, &ninds);
            if (consume_string(&p, "end_header\n")) {
                if (ninds == 0 || nverts == 0)  {
                    printf("ply parsing failed, could not determine number "
                           " of vertices or faces\n");
                    done = 1;
                    break;
                }
                state = PLY_VERTICES;
            }
            break;
        case PLY_VERTICES:
            res = parse_vertex(&p, vert, norm, color);
            if (!res) {
                done = 1;
                break;
            }
            (*vert_cb)(context, nverts, vert, norm, color);
            break;
        case PLY_INDICES:
            res = parse_indices(&p, inds);
            if (!res) {
                done = 1;
                break;
            }
            (*index_cb)(context, ninds, inds);

            break;
        }
    }

    free((void*)data);

    return success;
}
