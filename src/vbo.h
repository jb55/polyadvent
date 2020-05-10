#ifndef POLYADVENT_BUFFER_H
#define POLYADVENT_BUFFER_H

#include "gl.h"
#include "common.h"

typedef GLuint gpu_addr;

enum vertex_attr {
    va_position,
    va_normal,
    va_color,
    va_index,
    va_tex_coord,
    va_joint_ids,
    va_joint_weights,
    MAX_VERTEX_ATTRS
};

struct vbo {
    u32 type;
    int component_type;
    int components;
    gpu_addr handle;
};

struct num_elements {
    int num_elements;
};
#define mk_num_elements(x) ((struct num_elements){ .num_elements = x })

struct element_size {
    int element_size;
};
#define mk_element_size(x) ((struct element_size){ .element_size = x })

struct components {
    int components;
};
#define mk_components(c) ((struct components){ .components = c })

#define NUM_VBO_SLOTS 1

gpu_addr make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size);

struct vbo *init_vbo(struct vbo *);

struct vbo* make_float_vertex_buffer(struct vbo *vbo,
                                     const void *data,
                                     struct num_elements,
                                     struct components);

struct vbo* make_index_buffer(struct vbo *vbo,
                              const void *data,
                              struct num_elements);

struct vbo * make_uv_buffer(struct vbo *vbo,
                            const void *data,
                            struct num_elements,
                            struct components);

struct vbo *
make_int_vertex_buffer(struct vbo *vbo, const u32 *data,
                       struct num_elements num_elements,
                       struct components components);


void bind_uv_vbo(struct vbo *vbo, gpu_addr slot);
void bind_vbo(struct vbo *vbo, gpu_addr slot, GLenum type);
void free_vbo(struct vbo *vbo);

#endif /* POLYADVENT_BUFFER_H */
