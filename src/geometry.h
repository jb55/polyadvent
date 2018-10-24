
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "buffer.h"

struct buffer_geometry {
    struct vbo vertex_buffer;
    struct vbo index_buffer;
    struct vbo normal_buffer;
};

struct geometry {
    struct buffer_geometry buffer;
    int num_indices;
    int num_verts;
    float *vertices;
    float *normals;
    u32 *indices;
};

void make_buffer_geometry(struct geometry *geom);
void destroy_buffer_geometry(struct geometry *geom);

#endif /* GEOMETRY_H */
