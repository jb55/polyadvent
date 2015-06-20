
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "game.h"
#include "buffer.h"
#include "shader.h"
#include "debug.h"
#include "render.h"
#include "util.h"



//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

static const GLfloat g_vertex_buffer_data[] = {
  1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,    // v0-v1-v2-v3 front
  1, 1,-1,   1, 1, 1,   1,-1, 1,   1,-1,-1,    // v5-v0-v3-v4 right
 -1, 1, 1,   1, 1, 1,   1, 1,-1,  -1, 1,-1,    // v1-v0-v5-v6 top
 -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,    // v1-v6-v7-v2 left
  1,-1, 1,  -1,-1, 1,  -1,-1,-1,   1,-1,-1,    // v3-v2-v7-v4 bottom
 -1, 1,-1,   1, 1,-1,   1,-1,-1,  -1,-1,-1     // v4-v7-v6-v5 back
};


/* static const GLfloat g_element_buffer_normals[] = { */
/*   0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, // front */
/*   1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0, // right */
/*   0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0, // top */
/*  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0, // left */
/*   0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0, // bottom */
/*   0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1  // back */
/* }; */


static const GLushort g_element_buffer_data[] = {
   0, 1, 2,   0, 2, 3,    // front
   4, 5, 6,   4, 6, 7,    // right
   8, 9,10,   8,10,11,    // top
  12,13,14,  12,14,15,    // left
  16,17,18,  16,18,19,    // bottom
  20,21,22,  20,22,23
};

void
init_gl(struct test_resources * resources) {
  //glEnable(GL_DEPTH_TEST);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);

  /* wireframe_mode_on(); */

  // VBOs
  make_vertex_buffer(
    GL_ARRAY_BUFFER,
    g_vertex_buffer_data,
    sizeof(g_vertex_buffer_data),
    &resources->vertex_buffer
  );

  make_index_buffer(
    GL_ELEMENT_ARRAY_BUFFER,
    g_element_buffer_data,
    sizeof(g_element_buffer_data),
    &resources->element_buffer
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

  resources->uniforms.mvp
    = glGetUniformLocation(resources->program, "mvp");

  resources->attributes.position
    = (gpu_addr)glGetAttribLocation(resources->program, "position");

  assert(resources->program != 0);
}



void render (struct test_resources * resources) {
  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); //clear background screen to black
  glClear( GL_COLOR_BUFFER_BIT );

  //glMatrixMode(GL_MODELVIEW); //Switch to the drawing perspective
  //glLoadIdentity(); //Reset the drawing perspective

  glUseProgram(resources->program);

  glUniform1f(resources->uniforms.fade_factor,
              resources->fade_factor);

  glUniformMatrix4fv(resources->uniforms.mvp, 1, 0, resources->test_mvp);

  bind_vbo(&resources->vertex_buffer,
           resources->attributes.position);

  bind_ibo(&resources->element_buffer);

  glDrawElements(
    GL_TRIANGLE_STRIP,
    36,                 /* count */
    GL_UNSIGNED_SHORT,  /* type */
    (void*)0            /* element array buffer offset */
  );

  //glDisableVertexAttribArray(resources->attributes.position);
}

