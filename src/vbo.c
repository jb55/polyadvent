
#include "vbo.h"
#include "util.h"
#include <assert.h>
#include "gl.h"

gpu_addr
make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size) {
  gpu_addr buffer;
  check_gl();
  glGenBuffers(1, &buffer);
  check_gl();

  glBindBuffer(target, buffer);
  check_gl();

  glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
  check_gl();

  return buffer;
}

struct vbo*
make_index_buffer(GLenum target, const void *data, GLsizei buffer_size,
                  struct vbo *vbo) {
  vbo->components = 3;
  vbo->handle = make_buffer(target, data, buffer_size);
  vbo->type = GL_ELEMENT_ARRAY_BUFFER;
  return vbo;
}


struct vbo*
make_vertex_buffer(GLenum target, const void *data,
                   GLsizei buffer_size, struct vbo *vbo) {
  vbo->components = 3;
  vbo->handle = make_buffer(target, data, buffer_size);
  vbo->type = GL_ARRAY_BUFFER;
  return vbo;
}

struct vbo*
make_uv_buffer(GLenum target, const void *data,
               GLsizei buffer_size, struct vbo *vbo, int components) {
    vbo->components = components;
    vbo->handle = make_buffer(target, data, buffer_size);
    vbo->type = GL_ARRAY_BUFFER;
    return vbo;
}

void bind_ibo(struct vbo *vbo) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->handle);
}

struct vbo* init_vbo(struct vbo *vbo) {
    vbo->type = 0;
    vbo->handle = 0;
    vbo->components = 0;
    return vbo;
}

static void bind_vbo_internal(struct vbo *vbo, gpu_addr slot) {
  glEnableVertexAttribArray(slot);
  check_gl();
  glBindBuffer(vbo->type, vbo->handle);
  check_gl();
  glVertexAttribPointer(slot,              // attribute
                        vbo->components,   // size
                        GL_FLOAT,          // type
                        GL_FALSE,          // normalized?
                        0,   // stride
                        (void*)0           // array buffer offset
                        );
}

void bind_vbo(struct vbo *vbo, gpu_addr slot) {
    bind_vbo_internal(vbo, slot);
}
