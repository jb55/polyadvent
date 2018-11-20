
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "vbo.h"
#include "shader.h"
#include "id.h"

#define MAX_GEOMETRY 64

// -1 is uninitialized
struct geometry_id {
    struct id id;
};

struct geometry {
    struct vbo vertex;
    struct vbo index;
    struct vbo normal;
    struct vbo color;
    struct vbo tex_coord;

    float min[3];
    float max[3];

    int has_vbos;
    int num_uv_components;
    int num_indices;
    int num_verts;
};

struct make_geometry {
    float *vertices;
    float *normals;
    float *colors;
    int num_verts;

    u32 *indices;
    int num_indices;

    float *tex_coords;
    int num_uv_components;
};

struct geometry_manager {
    struct geometry geoms[MAX_GEOMETRY];
    int num_geometry;
};

void render_geometry(struct geometry *geom, struct attributes *,
                     struct gpu_program *current_program);
void bind_geometry(struct geometry *geom, struct attributes *);
void init_geometry(struct geometry *geom);
void init_make_geometry(struct make_geometry *mkgeom);
struct geometry_id make_buffer_geometry(struct make_geometry *mkgeom);
void destroy_buffer_geometry(struct geometry *geom);
void geometry_centroid(struct geometry *geom, float *v3);
void init_geometry_manager();
void init_geometry_id(struct geometry_id *);
struct geometry *new_geometry(struct geometry_id *);
struct geometry *get_geometry(struct geometry_id);
struct geometry *get_all_geometry(u32 *count);

#endif /* GEOMETRY_H */
