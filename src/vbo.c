
#include "vbo.h"
#include "util.h"
#include "debug.h"
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
  assert(buffer_size > 0);

  /* debug("buffer data %d\n", buffer_size); */
  glBufferData(target, buffer_size, buffer_data, GL_STATIC_DRAW);
  check_gl();

  return buffer;
}


static struct vbo*
make_vertex_buffer(struct vbo *vbo, const void *data,
                   GLenum type,
                   GLenum component_type,
                   struct element_size element_size,
                   struct num_elements num_elements,
                   struct components components)
{
    vbo->components = components.components;
    vbo->handle = make_buffer(type,
                              data,
                              num_elements.num_elements *
                              element_size.element_size *
                              components.components);
    vbo->type = type;
    vbo->component_type = component_type;
    return vbo;
}

struct vbo*
make_index_buffer(struct vbo *vbo, const void *data,
                  struct num_elements num_elements)
{
    vbo->handle = make_buffer(GL_ELEMENT_ARRAY_BUFFER,
                              data,
                              num_elements.num_elements * sizeof(u32));
    vbo->type = GL_ELEMENT_ARRAY_BUFFER;
    vbo->component_type = GL_INT;
    return vbo;
}

struct vbo* make_float_vertex_buffer(struct vbo *vbo, const void *data,
                                     struct num_elements num_elements,
                                     struct components components)
{
    return make_vertex_buffer(vbo,
                              data,
                              GL_ARRAY_BUFFER,
                              GL_FLOAT,
                              mk_element_size(sizeof(GLfloat)),
                              num_elements,
                              components);
}


struct vbo* make_int_vertex_buffer(struct vbo *vbo, const int *data,
                                   struct num_elements num_elements,
                                   struct components components)
{
    return make_vertex_buffer(vbo,
                              data,
                              GL_ARRAY_BUFFER,
                              GL_INT,
                              mk_element_size(sizeof(u32)),
                              num_elements,
                              components);
}


struct vbo * make_uv_buffer(struct vbo *vbo, const void *data,
                            struct num_elements num_elements,
                            struct components components)
{
    return make_vertex_buffer(vbo,
                              data,
                              GL_ARRAY_BUFFER,
                              GL_FLOAT,
                              mk_element_size(sizeof(float)),
                              num_elements,
                              components);
}


static void bind_ibo(struct vbo *vbo)
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo->handle);
}

struct vbo* init_vbo(struct vbo *vbo) {
    vbo->type = 0;
    vbo->handle = 0;
    vbo->components = 0;
    return vbo;
}

static void bind_vbo_internal(struct vbo *vbo, gpu_addr slot, GLenum type) {
  glEnableVertexAttribArray(slot);
  check_gl();
  glBindBuffer(vbo->type, vbo->handle);
  check_gl();

  // should use bind_ibo instead...
  assert(vbo->type != GL_ELEMENT_ARRAY_BUFFER);

  if (vbo->component_type == GL_INT) {
      glVertexAttribIPointer(slot,
                             vbo->components,
                             type,
                             0, // stride
                             (void*)0);
  }
  else {
      glVertexAttribPointer(slot,              // attribute
                            vbo->components,   // size
                            type,          // type
                            GL_FALSE,          // normalized?
                            0,   // stride
                            (void*)0           // array buffer offset
                            );
  }
}

void bind_vbo(struct vbo *vbo, gpu_addr slot, GLenum type) {
    if (vbo->type == GL_ELEMENT_ARRAY_BUFFER)
        return bind_ibo(vbo);
    bind_vbo_internal(vbo, slot, type);
}
