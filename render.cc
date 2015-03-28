
#include "gl.h"
#include "buffer.h"

// TESTING
static GLuint g_vertex_buffer;
static GLuint g_element_buffer;

static const GLfloat g_vertex_buffer_data[] = {
    -1.0f, -1.0f,
     1.0f, -1.0f,
    -1.0f,  1.0f,
     1.0f,  1.0f
};
static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };


void
init_gl() {
  //glEnable(GL_DEPTH_TEST);

  g_vertex_buffer = make_buffer(
      GL_ARRAY_BUFFER,
      g_vertex_buffer_data,
      sizeof(g_vertex_buffer_data)
  );

  g_element_buffer = make_buffer(
      GL_ELEMENT_ARRAY_BUFFER,
      g_element_buffer_data,
      sizeof(g_element_buffer_data)
  );
}


void
render () {
  glBindBuffer(GL_ARRAY_BUFFER, g_vertex_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_element_buffer);

  glDrawElements(
      GL_TRIANGLE_STRIP,  /* mode */
      4,                  /* count */
      GL_UNSIGNED_SHORT,  /* type */
      (void*)0            /* element array buffer offset */
  );
}
