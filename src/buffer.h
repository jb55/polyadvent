#ifndef POLYADVENT_BUFFER_H
#define POLYADVENT_BUFFER_H

#include "gl.h"
#include "common.h"

typedef GLuint gpu_addr;

struct attributes {
    gpu_addr position;
    gpu_addr normal;
    gpu_addr color;
    gpu_addr tex_coord;
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
void bind_ibo(struct vbo *vbo);

#endif /* POLYADVENT_BUFFER_H */
