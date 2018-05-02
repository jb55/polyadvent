
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "game.h"
#include "mat4.h"
#include "vec3.h"
#include "buffer.h"
#include "buffer_geometry.h"
#include "shader.h"
#include "geometry.h"
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
  0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,    // v0-v0.5-v2-v3 front
  0.5, 0.5,-0.5,   0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5,-0.5,-0.5,    // v5-v0-v3-v4 right
 -0.5, 0.5, 0.5,   0.5, 0.5, 0.5,   0.5, 0.5,-0.5,  -0.5, 0.5,-0.5,    // v0.5-v0-v5-v6 top
 -0.5, 0.5, 0.5,  -0.5, 0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5, 0.5,    // v0.5-v6-v7-v2 left
  0.5,-0.5, 0.5,  -0.5,-0.5, 0.5,  -0.5,-0.5,-0.5,   0.5,-0.5,-0.5,    // v3-v2-v7-v4 bottom
 -0.5, 0.5,-0.5,   0.5, 0.5,-0.5,   0.5,-0.5,-0.5,  -0.5,-0.5,-0.5     // v4-v7-v6-v5 back
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
init_gl(struct resources *resources, int width, int height) {
  float tmp_matrix[16];
  glEnable(GL_DEPTH_TEST);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

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

  // camera
  mat4_perspective(90 /* fov */, (float)width / height, 1, 1000, resources->camera_persp);

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

  resources->uniforms.tscale
    = glGetUniformLocation(resources->program, "tscale");

  resources->uniforms.local
    = glGetUniformLocation(resources->program, "local");

  resources->attributes.normal
    = (gpu_addr)glGetAttribLocation(resources->program, "normal");

  resources->attributes.position
    = (gpu_addr)glGetAttribLocation(resources->program, "position");

  assert(resources->program != 0);
}


static mat4 *
calc_normals(mat4 *mvp, mat4 *normal) {
  mat4_inverse(mvp, normal);
  mat4_transpose(normal, normal);
  return normal;
}


static void
recalc_normals(GLint norm_uniform, mat4 *mvp, mat4 *normal) {
  mat4 *calc = calc_normals(mvp, normal);
  glUniformMatrix4fv(norm_uniform, 1, 0, calc);
  check_gl();
}


static float tmp_matrix[MAT4_ELEMS] = { 0 };


static void
render_cube (struct resources * resources) {
  bind_vbo(&resources->vertex_buffer,
           resources->attributes.position);

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


static void render_geom (struct resources *res,
                         struct geometry *geom,
                         GLenum type) {
  struct attributes *attrs = &res->attributes;

  bind_vbo(&geom->buffer.vertex_buffer, attrs->position);
  bind_vbo(&geom->buffer.normal_buffer, attrs->normal);
  bind_ibo(&geom->buffer.index_buffer);

  glDrawElements(type,
                 geom->num_indices, /* count */
                 GL_UNSIGNED_INT,    /* type */
                 (void*)0            /* element array buffer offset */
                 );
}


void render (struct game *game, struct geometry *geom) {
  glClearColor( 0.5294f, 0.8078f, 0.9216f, 1.0f ); //clear background screen to black
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  check_gl();

  static float id[MAT4_ELEMS] = { 0 };
  mat4_id(id);
  struct resources *res = &game->test_resources;

  mat4 *mvp = res->test_mvp;
  mat4 *persp = res->camera_persp;
  mat4 *light = res->light_dir;

  struct node *player = &res->player;
  struct node *camera = &res->camera;

  float fade_factor = res->fade_factor;

  glUseProgram(res->program);

  /* static float v3[] = { 1, 1, 0 }; */
  /* v3[1] = fade_factor * 1.4f; */
  /* mat4_rotate(mvp, 0.004f, v3, mvp); */
  /* printf("camera_pos %f %f %f", camera_pos[0], camera_pos[1], camera_pos[2]); */
  /* mat4_print(camera->mat); */
  /* node_recalc(&res->camera); */
  /* mat4_multiply(persp, camera->mat, mvp); */
  mat4_inverse(camera->mat, tmp_matrix);
  mat4_multiply(persp, tmp_matrix, mvp);
  /* mat4_multiply(mvp, tmp_matrix, tmp_matrix); */

  glUniform3f(res->uniforms.light_dir, light[0], light[1], light[2]);
  glUniform1f(res->uniforms.fade_factor, fade_factor);
  glUniform1f(res->uniforms.tscale, res->uniforms.tscale);

  //player
  mat4_multiply(mvp, player->mat, tmp_matrix);
  glUniformMatrix4fv(res->uniforms.mvp, 1, 0, tmp_matrix);
  glUniformMatrix4fv(res->uniforms.local, 1, 0, player->local);
  /* mat4_multiply(persp, tmp_matrix, mvp); */
  /* mat4_print(player->mat); */
  render_cube(res);

  glUniformMatrix4fv(res->uniforms.local, 1, 0, id);

  // terrain
  glUniformMatrix4fv(res->uniforms.mvp, 1, 0, mvp);
  render_geom(res, geom, GL_TRIANGLES);
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  /* render_geom(res, geom, GL_TRIANGLES); */
}
