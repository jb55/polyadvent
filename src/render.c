
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "game.h"
#include "buffer.h"
#include "shader.h"
#include "debug.h"
#include "render.h"

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




void
init_gl(TestResources * resources) {
  //glEnable(GL_DEPTH_TEST);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

  // VBOs
  resources->vertex_buffer = make_buffer(
      GL_ARRAY_BUFFER,
      g_vertex_buffer_data,
      sizeof(g_vertex_buffer_data)
  );
  resources->element_buffer = make_buffer(
      GL_ELEMENT_ARRAY_BUFFER,
      g_element_buffer_data,
      sizeof(g_element_buffer_data)
  );

  // Shaders
  resources->vertex_shader = make_shader(
      GL_VERTEX_SHADER,
      SHADER("test.v.glsl")
  );
  assert(resources->vertex_shader != 0);
  resources->fragment_shader = make_shader(
      GL_FRAGMENT_SHADER,
      SHADER("test.f.glsl")
  );
  assert(resources->fragment_shader != 0);


  // Shader program
  resources->program = make_program(resources->vertex_shader,
                                     resources->fragment_shader);
  assert(resources->program != 0);


  // Program variables
  resources->uniforms.fade_factor
      = glGetUniformLocation(resources->program, "fade_factor");

  resources->attributes.position
      = glGetAttribLocation(resources->program, "position");

  assert(resources->program != 0);
}


void
render (TestResources * resources) {
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); //clear background screen to black
  glClear( GL_COLOR_BUFFER_BIT );

  //glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
  //glLoadIdentity(); //Reset the drawing perspective

  glUseProgram(resources->program);

  /* glUniform1f(resources->uniforms.fade_factor, */
  /*             resources->fade_factor) */;

  glBindBuffer(GL_ARRAY_BUFFER,
               resources->vertex_buffer);

  glVertexAttribPointer(
    (GLuint)resources->attributes.position,   /* attribute */
    2,                                /* size */
    GL_FLOAT,                         /* type */
    GL_FALSE,                         /* normalized? */
    sizeof(GLfloat)*2,                /* stride */
    (void*)0                          /* array buffer offset */
  );

  glEnableVertexAttribArray((GLuint)resources->attributes.position);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,
               resources->element_buffer);

  glDrawElements(
    GL_TRIANGLE_STRIP,  /* mode */
    4,                  /* count */
    GL_UNSIGNED_SHORT,  /* type */
    (void*)0            /* element array buffer offset */
  );

  glDisableVertexAttribArray((GLuint)resources->attributes.position);
}
