
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "file.h"
#include "common.h"
#include "ply.h"
#include "vec3.h"

/* void parse_vertex( */


enum ply_state {
    PLY_MAGIC,
    PLY_HEADER,
    PLY_VERTICES,
    PLY_INDICES
};

static void next_line(const char **cursor) {
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

    return *nverts != 0;
}


static inline int parse_vertex(const char **cursor, float *v, float *n, u8 *c) {
    static float st[2];

    int matched =
        sscanf(*cursor, "%f %f %f %f %f %f %f %f %hhu %hhu %hhu",
            &v[0], &v[1], &v[2],
            &n[0], &n[1], &n[2],
            &st[0], &st[1],
            &c[0], &c[1], &c[2]);

    if (matched == 11)
        return 1;

    matched =
        sscanf(*cursor, "%f %f %f %f %f %f %hhu %hhu %hhu",
               &v[0], &v[1], &v[2],
               &n[0], &n[1], &n[2],
               &c[0], &c[1], &c[2]);

    if (matched == 9)
        return 1;

    return 0;
}


static int parse_indices(const char **cursor, int *inds) {
    // NOTE: only support tris inds for now
    int matched =
        sscanf(*cursor, "3 %d %d %d", &inds[0], &inds[1], &inds[2]);

    return matched == 3;
}

static int parse_header(const char **cursor, int *nverts, int *ninds) {
    int ok = 0;
    ok = parse_element(cursor, "vertex", nverts);
    if (ok) return 1;

    ok = parse_element(cursor, "face", ninds);
    if (ok) return 1;

    ok = parse_element(cursor, "face", ninds);
    if (ok) return 1;


    return ok;
}

static int parse_magic(const char **cursor) {
    return consume_string(cursor, "ply");
}


int parse_ply(const char *filename, geometry_id *geom_id) {
    size_t len;
    int success = 0;
    int nverts = 0;
    int ninds = 0;
    int done = 0;
    int res = 0;
    int cvert = 0;
    int cind = 0;

    enum ply_state state = PLY_MAGIC;
    const char *data = file_contents(filename, &len);
    const char *p = data;
    struct make_geometry mkgeom;
    init_make_geometry(&mkgeom);

    float vert[3], norm[3], min[3], max[3];
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
            if (consume_string(&p, "end_header")) {
                if (ninds == 0 || nverts == 0)  {
                    printf("ply parsing failed, could not determine number "
                           " of vertices or faces\n");
                    done = 1;
                    break;
                }

                mkgeom.vertices = calloc(nverts * 3, sizeof(*mkgeom.vertices));
                mkgeom.normals  = calloc(nverts * 3, sizeof(*mkgeom.normals));
                mkgeom.colors   = calloc(nverts * 3, sizeof(*mkgeom.colors));
                mkgeom.indices  = calloc(ninds * 3, sizeof(*mkgeom.indices));

                state = PLY_VERTICES;
            }
            break;
        case PLY_VERTICES:
            res = parse_vertex(&p, vert, norm, color);
            if (!res) {
                printf("failed parsing verts\n");
                done = 1;
                break;
            }

            // compute bounding box as we go
            if (cvert == 0) {
                vec3_copy(vert, min);
                vec3_copy(vert, max);
            }
            else {
                vec3_min(vert, min, min);
                vec3_max(vert, max, max);
            }

            mkgeom.vertices[cvert * 3]     = vert[0];
            mkgeom.vertices[cvert * 3 + 1] = vert[1];
            mkgeom.vertices[cvert * 3 + 2] = vert[2];

            mkgeom.normals[cvert * 3]     = norm[0];
            mkgeom.normals[cvert * 3 + 1] = norm[1];
            mkgeom.normals[cvert * 3 + 2] = norm[2];

            mkgeom.colors[cvert * 3]     = color[0] / 255.0;
            mkgeom.colors[cvert * 3 + 1] = color[1] / 255.0;
            mkgeom.colors[cvert * 3 + 2] = color[2] / 255.0;

            cvert++;

            if (cvert == nverts)
                state = PLY_INDICES;

            break;
        case PLY_INDICES:
            res = parse_indices(&p, inds);
            if (!res) {
                printf("failed parsing indices\n");
                done = 1;
                break;
            }

            mkgeom.indices[cind * 3]     = inds[0];
            mkgeom.indices[cind * 3 + 1] = inds[1];
            mkgeom.indices[cind * 3 + 2] = inds[2];

            cind++;

            if (cind == ninds) {
                success = 1;
                done = 1;
            }

            break;
        }

        // next line
        if (p >= data + len) {
            /* printf("got here, state %d cind %d ninds %d over %ld\n", */
            /*        state, cind, ninds, p - (data + len)); */
            done = 1;
        }

        if (!done)
            next_line(&p);
    }

    free((void*)data);

    if (success) {
        struct geometry *geom = new_geometry(geom_id);
        assert(geom);

        mkgeom.num_indices = ninds * 3;
        mkgeom.num_verts = nverts * 3;

        make_buffer_geometry(&mkgeom, geom_id);

        vec3_copy(min, geom->min);
        vec3_copy(max, geom->max);

        free(mkgeom.vertices);
        free(mkgeom.normals);
        free(mkgeom.colors);
        free(mkgeom.indices);
    }

    return success;
}
