
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "buffer.h"
#include "shader.h"

struct geometry {
    struct vbo vbos[n_vertex_attrs];
    float min[3];
    float max[3];
    int num_uv_components;
    int num_indices;
    int num_verts;
    float *vertices;
    float *normals;
    u32 *indices;
    float *colors;
    float *tex_coords;
    int *joint_ids;
    float *joint_weights;
};

void render_geometry(struct geometry *geom, gpu_addr *vertex_attrs,
                     struct gpu_program *current_program);
void bind_geometry(struct geometry *geom, gpu_addr *vertex_attrs);
void init_geometry(struct geometry *geom);
void make_buffer_geometry(struct geometry *geom);
void destroy_buffer_geometry(struct geometry *geom);
void geometry_centroid(struct geometry *geom, float *v3);

#endif /* GEOMETRY_H */
