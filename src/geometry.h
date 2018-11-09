
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "buffer.h"
#include "shader.h"

struct buffer_geometry {
    struct vbo vertex;
    struct vbo index;
    struct vbo normal;
    struct vbo color;
    struct vbo tex_coord;
};

struct geometry {
    struct buffer_geometry vbos;
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
};

void render_geometry(struct geometry *geom, struct attributes *,
                     struct gpu_program *current_program);
void bind_geometry(struct geometry *geom, struct attributes *);
void init_geometry(struct geometry *geom);
void make_buffer_geometry(struct geometry *geom);
void destroy_buffer_geometry(struct geometry *geom);
void geometry_centroid(struct geometry *geom, float *v3);

#endif /* GEOMETRY_H */
