#ifndef POLYADVENT_BUFFER_H
#define POLYADVENT_BUFFER_H

#include "gl.h"
#include "common.h"

typedef GLuint gpu_addr;

#define VERT_ATTRS 6

enum vertex_attr {
    va_position,
    va_normal,
    va_color,
    va_index,
    va_tex_coord,
    va_joint_ids,
    va_joint_weights,
    n_vertex_attrs
};

struct vbo {
  u32 type;
  int components;
  gpu_addr handle;
};

#define NUM_VBO_SLOTS 1

gpu_addr make_buffer(GLenum target, const void *buffer_data,
                     GLsizei buffer_size);

struct vbo *
make_vertex_buffer(GLenum target, const void *buffer_data,
                   GLsizei buffer_size, struct vbo *vbo);

struct vbo *
make_index_buffer(GLenum target, const void *buffer_data,
                  GLsizei buffer_size, struct vbo *vbo);

struct vbo *
make_uv_buffer(GLenum target, const void *data,
               GLsizei buffer_size, struct vbo *vbo, int components);

void bind_uv_vbo(struct vbo *vbo, gpu_addr slot);
void bind_vbo(struct vbo *vbo, gpu_addr slot);

#endif /* POLYADVENT_BUFFER_H */
