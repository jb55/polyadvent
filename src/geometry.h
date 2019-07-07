
#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "common.h"
#include "vbo.h"
#include "shader.h"
#include "resource.h"

#define DEF_NUM_GEOMETRY 64
#define MAX_GEOMETRY 1024

// -1 is uninitialized
typedef struct resource_id geometry_id;

struct geometry {
    struct vbo vbos[n_vertex_attrs];
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

    u32 *joint_ids;
    int num_joint_ids;

    float *joint_weights;
    int num_weights;
};

struct geometry_manager {
    struct geometry geoms[MAX_GEOMETRY];
    int num_geometry;
};

void render_geometry(struct geometry *geom, gpu_addr *vertex_attrs,
                     struct gpu_program *current_program);
void bind_geometry(struct geometry *geom, gpu_addr *vertex_attrs);
void init_geometry(struct geometry *geom);
void init_make_geometry(struct make_geometry *mkgeom);
void free_make_geometry(struct make_geometry *mkgeom);
void make_buffer_geometry(struct make_geometry *mkgeom, struct geometry *geom);
void destroy_buffer_geometry(geometry_id *geom);
void geometry_centroid(struct geometry *geom, float *v3);
void init_geometry_manager();
void init_geometry_id(geometry_id *);
struct geometry *new_geometry(geometry_id *);
struct geometry *get_geometry(geometry_id *);
struct geometry *get_all_geometry(u32 *count, geometry_id **ids);
void destroy_geometry(geometry_id *geom_id);
struct resource_manager *get_geometry_manager();

extern struct resource_manager geom_manager;

static inline struct geometry *static_geometry()
{
    return (struct geometry *)geom_manager.resources;
}

#endif /* GEOMETRY_H */
