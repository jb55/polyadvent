
#include "buffer.h"

/*
 * Functions for creating OpenGL objects:
 */

gpu_addr
make_buffer(GLenum target, const void *buffer_data, GLsizei buffer_size) {
  gpu_addr buffer;
  glGenBuffers(1, &buffer);
  glBindBuffer(target, buffer);
  glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
  return buffer;
}


struct vbo*
make_index_buffer(GLenum target, const void *data, GLsizei buffer_size,
                  struct vbo *vbo) {
  vbo->components = 2;
  vbo->handle = make_buffer(target, data, buffer_size);
  vbo->type = GL_ELEMENT_ARRAY_BUFFER;
  return vbo;
}


struct vbo*
make_vertex_buffer(GLenum target, const void *data,
                   GLsizei buffer_size, struct vbo *vbo) {
  vbo->components = 2;
  vbo->handle = make_buffer(target, data, buffer_size);
  vbo->type = GL_ARRAY_BUFFER;
  return vbo;
}

void bind_ibo(struct vbo *vbo) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->handle);
}

void bind_vbo(struct vbo *vbo, gpu_addr slot) {
  glEnableVertexAttribArray(slot);
  glBindBuffer(vbo->type, vbo->handle);
  glVertexAttribPointer(slot,              // attribute
                        vbo->components,   // size
                        GL_FLOAT,          // type
                        GL_FALSE,          // normalized?
                        sizeof(GLfloat)*2, // stride
                        (void*)0           // array buffer offset
                        );
}
