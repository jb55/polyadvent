
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "game.h"
#include "mat4/mat4.h"
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

static const GLfloat cube_vertices[] = {
  1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,    // v0-v1-v2-v3 front
  1, 1,-1,   1, 1, 1,   1,-1, 1,   1,-1,-1,    // v5-v0-v3-v4 right
 -1, 1, 1,   1, 1, 1,   1, 1,-1,  -1, 1,-1,    // v1-v0-v5-v6 top
 -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,    // v1-v6-v7-v2 left
  1,-1, 1,  -1,-1, 1,  -1,-1,-1,   1,-1,-1,    // v3-v2-v7-v4 bottom
 -1, 1,-1,   1, 1,-1,   1,-1,-1,  -1,-1,-1     // v4-v7-v6-v5 back
};


static const GLfloat cube_normals[] = {
  0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, // front
  1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0, // right
  0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0, // top
 -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0, // left
  0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0, // bottom
  0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1  // back
};


static const GLushort cube_indices[] = {
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
    cube_vertices,
    sizeof(cube_vertices),
    &resources->vertex_buffer
  );

  // cube normals
  make_vertex_buffer(
    GL_ARRAY_BUFFER,
    cube_normals,
    sizeof(cube_normals),
    &resources->normal_buffer
  );

  // cube indices
  make_index_buffer(
    GL_ELEMENT_ARRAY_BUFFER,
    cube_indices,
    sizeof(cube_indices),
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

  resources->uniforms.light_dir
    = glGetUniformLocation(resources->program, "light_dir");

  resources->uniforms.normal_matrix
    = glGetUniformLocation(resources->program, "normal_matrix");

  resources->uniforms.mvp
    = glGetUniformLocation(resources->program, "mvp");

  resources->attributes.normal
    = (gpu_addr)glGetAttribLocation(resources->program, "normal");

  resources->attributes.position
    = (gpu_addr)glGetAttribLocation(resources->program, "position");

  assert(resources->program != 0);
}

static mat4
*calc_normals(mat4 *mvp, mat4 *normal) {
  mat4_inverse(mvp, normal);
  mat4_transpose(normal, normal);
  return normal;
}

static void
recalc_normals(GLint norm_uniform, mat4 *mvp, mat4 *normal) {
  mat4 *calc = calc_normals(mvp, normal);
  glUniformMatrix4fv(norm_uniform, 1, 0, calc);
}


void render (struct test_resources * resources) {
  float *mvp = &resources->test_mvp[0];
  float fade_factor = resources->fade_factor;

  glClearColor( 0.0f, 0.0f, 0.0f, 1.0f ); //clear background screen to black
  glClear( GL_COLOR_BUFFER_BIT );

  static float v3[] = { 1, 1, 0 };
  //v3[1] = (float)(fade_factor * 0.4);
  mat4_rotate(mvp, (float)0.004, v3, mvp);

  recalc_normals(resources->uniforms.normal_matrix, mvp,
                 &resources->normal_matrix[0]);

  glUseProgram(resources->program);

  glUniform3f(resources->uniforms.light_dir, -1, 1, (float)-0.09);
  glUniform1f(resources->uniforms.fade_factor, fade_factor);
  glUniformMatrix4fv(resources->uniforms.mvp, 1, 0, resources->test_mvp);

  bind_vbo(&resources->vertex_buffer,
           resources->attributes.position);

  bind_vbo(&resources->normal_buffer,
           resources->attributes.normal);

  bind_ibo(&resources->element_buffer);

  glDrawElements(
    GL_TRIANGLES,
    ARRAY_SIZE(cube_indices), /* count */
    GL_UNSIGNED_SHORT,  /* type */
    (void*)0            /* element array buffer offset */
  );

  //glDisableVertexAttribArray(resources->attributes.position);
}

