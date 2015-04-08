
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "buffer.h"
#include "shader.h"
#include "debug.h"

/*
 * Global data used by our render callback:
 */
static const GLfloat g_vertex_buffer_data[] = {
    -0.5f, -0.5f,
     0.5f, -0.5f,
    -0.5f,  0.6f,
     0.5f,  0.5f
};
static const GLushort g_element_buffer_data[] = { 0, 1, 2, 3 };


/*
 * Global data used by our render callback:
 */
static struct {
    GLuint vertex_buffer, element_buffer;
    GLuint textures[2];
    GLuint vertex_shader, fragment_shader, program;

    struct {
        GLint fade_factor;
        GLint textures[2];
    } uniforms;

    struct {
        GLint position;
    } attributes;

    GLfloat fade_factor;
} g_resources;



void
init_gl() {
  //glEnable(GL_DEPTH_TEST);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

  // VBOs
  g_resources.vertex_buffer = make_buffer(
      GL_ARRAY_BUFFER,
      g_vertex_buffer_data,
      sizeof(g_vertex_buffer_data)
  );
  g_resources.element_buffer = make_buffer(
      GL_ELEMENT_ARRAY_BUFFER,
      g_element_buffer_data,
      sizeof(g_element_buffer_data)
  );

  // Shaders
  g_resources.vertex_shader = make_shader(
      GL_VERTEX_SHADER,
      SHADER("test.v.glsl")
  );
  assert(g_resources.vertex_shader != 0);
  g_resources.fragment_shader = make_shader(
      GL_FRAGMENT_SHADER,
      SHADER("test.f.glsl")
  );
  assert(g_resources.fragment_shader != 0);


  // Shader program
  g_resources.program = make_program(g_resources.vertex_shader,
                                     g_resources.fragment_shader);
  assert(g_resources.program != 0);


  // Program variables
  g_resources.uniforms.fade_factor
      = glGetUniformLocation(g_resources.program, "fade_factor");

  g_resources.attributes.position
      = glGetAttribLocation(g_resources.program, "position");

  assert(g_resources.program != 0);
}


void
render () {
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); //clear background screen to black
  glClear( GL_COLOR_BUFFER_BIT );

  //glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
  //glLoadIdentity(); //Reset the drawing perspective

  glUseProgram(g_resources.program);

  glBindBuffer(GL_ARRAY_BUFFER, g_resources.vertex_buffer);
  glVertexAttribPointer(
    g_resources.attributes.position,  /* attribute */
    2,                                /* size */
    GL_FLOAT,                         /* type */
    GL_FALSE,                         /* normalized? */
    sizeof(GLfloat)*2,                /* stride */
    (void*)0                          /* array buffer offset */
  );
  glEnableVertexAttribArray(g_resources.attributes.position);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_resources.element_buffer);
  glDrawElements(
    GL_TRIANGLE_STRIP,  /* mode */
    4,                  /* count */
    GL_UNSIGNED_SHORT,  /* type */
    (void*)0            /* element array buffer offset */
  );

  glDisableVertexAttribArray(g_resources.attributes.position);
}
