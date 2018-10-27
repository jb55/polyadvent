
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "buffer.h"

struct buffer_geometry {
    struct vbo vertex;
    struct vbo index;
    struct vbo normal;
    struct vbo color;
};

struct geometry {
    struct buffer_geometry vbos;
    int num_indices;
    int num_verts;
    float *vertices;
    float *normals;
    u32 *indices;
    float *colors;
};

void render_geometry(struct geometry *geom, struct attributes *);
void init_geometry(struct geometry *geom);
void make_buffer_geometry(struct geometry *geom);
void destroy_buffer_geometry(struct geometry *geom);

#endif /* GEOMETRY_H */
