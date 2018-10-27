
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "game.h"
#include "mat4.h"
#include "vec3.h"
#include "buffer.h"
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
  0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,    // v0-v1-v2-v3 front
  0.5, 0.5,-0.5,   0.5, 0.5, 0.5,   0.5,-0.5, 0.5,   0.5,-0.5,-0.5,    // v5-v0-v3-v4 right
  -0.5, 0.5, 0.5,   0.5, 0.5, 0.5,   0.5, 0.5,-0.5,  -0.5, 0.5,-0.5,   // v1-v0-v5-v6 top
  -0.5, 0.5, 0.5,  -0.5, 0.5,-0.5,  -0.5,-0.5,-0.5,  -0.5,-0.5, 0.5,   // v1-v6-v7-v2 left
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
	struct shader vertex, terrain_vertex, fragment, fragment_smooth;
	float tmp_matrix[16];
	int ok = 0;

	glEnable(GL_DEPTH_TEST);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);

	// Shaders
	ok = make_shader(GL_VERTEX_SHADER, SHADER("vertex-color.glsl"), &vertex);
	assert(ok && "vertex-color shader");

    ok = make_shader(GL_VERTEX_SHADER, SHADER("terrain.glsl"), &terrain_vertex);
	assert(ok && "terrain vertex shader");

	ok = make_shader(GL_FRAGMENT_SHADER, SHADER("test.f.glsl"), &fragment);
	assert(ok && "default fragment shader");

	// camera
	mat4_perspective(90 /* fov */,
			 (float)width / height,
			 1,
			 5000,
			 resources->camera_persp);

	// Shader program
	ok = make_program(&terrain_vertex, &fragment, &resources->terrain_program);
	assert(ok && "terrain program");
    check_gl();

	ok = make_program(&vertex, &fragment, &resources->program);
	assert(ok && "vertex-color program");
    check_gl();

    GLuint programs[] =
        { resources->terrain_program.handle
        , resources->program.handle
        };

    // uniforms shared between all shaders
    for (size_t i = 0; i < ARRAY_SIZE(programs); ++i) {
        GLuint handle = programs[i];

        // Program variables
        resources->uniforms.camera_position =
            glGetUniformLocation(handle, "camera_position");

        resources->uniforms.light_intensity =
            glGetUniformLocation(handle, "light_intensity");

        resources->uniforms.light_dir =
            glGetUniformLocation(handle, "light_dir");

        resources->uniforms.world =
            glGetUniformLocation(handle, "world");

        resources->uniforms.fog_on =
            glGetUniformLocation(handle, "fog_on");

        resources->uniforms.diffuse_on =
            glGetUniformLocation(handle, "diffuse_on");

        resources->uniforms.mvp =
            glGetUniformLocation(handle, "mvp");

        resources->uniforms.model_view =
            glGetUniformLocation(handle, "model_view");

        resources->uniforms.normal_matrix =
            glGetUniformLocation(handle, "normal_matrix");

        resources->attributes.normal =
            (gpu_addr)glGetAttribLocation(handle, "normal");

        resources->attributes.position =
            (gpu_addr)glGetAttribLocation(handle, "position");

    }

    // TODO: auto-generate these somehow?
	resources->attributes.color =
		(gpu_addr)glGetAttribLocation(resources->program.handle, "color");

    check_gl();
}


static void
recalc_normals(GLint nm_uniform, mat4 *model_view, mat4 *normal) {
  mat4_inverse(model_view, normal);
  mat4_transpose(normal, normal);
  /* mat4_copy(model_view, normal); */
  glUniformMatrix4fv(nm_uniform, 1, 0, normal);
}



void render (struct game *game) {
    float adjust = game->test_resources.light_intensity[0];
    glClearColor( 0.5294f * adjust, 0.8078f * adjust, 0.9216f * adjust, 1.0f ); //clear background screen to black
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    check_gl();

    static float id[MAT4_ELEMS] = { 0 };
    static float view[MAT4_ELEMS] = { 0 };
    static float view_proj[MAT4_ELEMS] = { 0 };
    static float normal_matrix[MAT4_ELEMS] = { 0 };
    static float model_view[MAT4_ELEMS] = { 0 };
    mat4_id(id);
    mat4_id(model_view);
    struct resources *res = &game->test_resources;

    mat4 *mvp = res->test_mvp;
    mat4 *persp = res->camera_persp;
    mat4 *light = res->light_dir;
    mat4 *light_intensity = res->light_intensity;

    struct node *camera = &res->camera;

    struct entity *entities[] =
        { &game->terrain.entity
        , &game->test_resources.player
        };

    for (size_t i = 0; i < ARRAY_SIZE(entities); ++i) {
        struct entity *entity = entities[i];
        // TODO this is a bit wonky, refactor this
        if (i == 0)
            glUseProgram(game->test_resources.terrain_program.handle);
        else if (i == 1)
            glUseProgram(game->test_resources.program.handle);
        check_gl();

        mat4_inverse(camera->mat, view);
        mat4_multiply(persp, view, view_proj);

        glUniform3f(res->uniforms.camera_position,
                    camera->mat[M_X],
                    camera->mat[M_Y],
                    camera->mat[M_Z]);

        glUniform1i(res->uniforms.fog_on, res->fog_on);
        glUniform1i(res->uniforms.diffuse_on, res->diffuse_on);
        glUniform3f(res->uniforms.light_dir, light[0], light[1], light[2]);
        glUniform3f(res->uniforms.light_intensity,
                    light_intensity[0], light_intensity[1], light_intensity[2]);

        mat4_multiply(view_proj, entity->node.mat, mvp);
        mat4_copy(entity->node.mat, model_view);

        glUniformMatrix4fv(res->uniforms.mvp, 1, 0, mvp);
        glUniformMatrix4fv(res->uniforms.model_view, 1, 0, model_view);
        glUniformMatrix4fv(res->uniforms.world, 1, 0, entity->node.mat);

        recalc_normals(res->uniforms.normal_matrix, model_view, normal_matrix);

        render_geom(res, &entity->model.geom, GL_TRIANGLES);

        check_gl();
    }


    //player
    // y tho

    // terrain

    /* glUniformMatrix4fv(res->uniforms.mvp, 1, 0, mvp); */
    /* glUniformMatrix4fv(res->uniforms.model_view, 1, 0, id); */
    /* glUniformMatrix4fv(res->uniforms.world, 1, 0, id); */
    /* glUniformMatrix4fv(res->uniforms.normal_matrix, 1, 0, id); */
    /* recalc_normals(res->uniforms.normal_matrix, model_view, normal_matrix); */
    /* render_geom(res, geom, GL_TRIANGLES); */
    /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
    /* render_geom(res, geom, GL_TRIANGLES); */
}
