
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
    static char buffer[32];

    snprintf(buffer, sizeof(buffer), "element %s ", element);

    ok = consume_string(cursor, buffer);

    if (!ok)
        return 0;

    *nverts = atoi(*cursor);

    skip_line(cursor);

    return *nverts != 0;
}


static inline int parse_vertex(const char **cursor, float *v, float *n, u8 *c) {
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
    ok |= parse_element(cursor, "face", ninds);

    if (ok)
        return 0;

    skip_line(cursor);
    return 0;
}

static int parse_magic(const char **cursor) {
    return consume_string(cursor, "ply\n");
}


int parse_ply(const char *filename, struct geometry *geom) {
    size_t len;
    int nverts = 0;
    int ninds = 0;
    int done = 0;
    int res = 0;
    int cvert = 0;
    int cind = 0;

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
                done = 1;
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

                geom->vertices = calloc(nverts * 3, sizeof(*geom->vertices));
                geom->normals  = calloc(nverts * 3, sizeof(*geom->normals));
                geom->colors   = calloc(nverts * 3, sizeof(*geom->colors));
                geom->indices  = calloc(ninds * 3, sizeof(*geom->indices));

                state = PLY_VERTICES;
            }
            break;
        case PLY_VERTICES:
            res = parse_vertex(&p, vert, norm, color);
            if (!res) {
                done = 1;
                break;
            }

            geom->vertices[cvert * 3]     = vert[0];
            geom->vertices[cvert * 3 + 1] = vert[1];
            geom->vertices[cvert * 3 + 2] = vert[2];

            geom->normals[cvert * 3]     = norm[0];
            geom->normals[cvert * 3 + 1] = norm[1];
            geom->normals[cvert * 3 + 2] = norm[2];

            geom->colors[cvert * 3]     = color[0] / 255.0;
            geom->colors[cvert * 3 + 1] = color[1] / 255.0;
            geom->colors[cvert * 3 + 2] = color[2] / 255.0;

            cvert++;

            if (cvert == nverts)
                state = PLY_INDICES;

            break;
        case PLY_INDICES:
            res = parse_indices(&p, inds);
            if (!res) {
                done = 1;
                break;
            }

            geom->indices[cind * 3]     = inds[0];
            geom->indices[cind * 3 + 1] = inds[1];
            geom->indices[cind * 3 + 2] = inds[2];

            cind++;

            if (cind == ninds)
                done = 1;

            break;
        }
    }

    free((void*)data);

    geom->num_indices = ninds * 3;
    geom->num_verts = nverts * 3;

    make_buffer_geometry(geom);

    return state == PLY_INDICES;
}
