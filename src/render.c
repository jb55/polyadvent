
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "gl.h"
#include "game.h"
#include "mat4.h"
#include "vec3.h"
#include "vbo.h"
#include "shader.h"
#include "geometry.h"
#include "debug.h"
#include "render.h"
#include "skybox.h"
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

static const float bias_matrix[] = {
  0.5, 0.0, 0.0, 0.0,
  0.0, 0.5, 0.0, 0.0,
  0.0, 0.0, 0.5, 0.0,
  0.5, 0.5, 0.5, 1.0
};

void
init_gl(struct resources *resources, int width, int height) {
    struct shader vertex, terrain_vertex, terrain_geom, fragment, fragment_smooth;
    struct shader terrain_teval, terrain_tc;
    float tmp_matrix[16];
    int ok = 0;

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glEnable(GL_MULTISAMPLE);
check_gl();

	// Shaders
	ok = make_shader(GL_VERTEX_SHADER, SHADER("vertex-color.glsl"), &vertex);
	assert(ok && "vertex-color shader");

    ok = make_shader(GL_VERTEX_SHADER, SHADER("terrain.v.glsl"),
                     &terrain_vertex);
	assert(ok && "terrain vertex shader");
	check_gl();

    /* ok = make_shader(GL_GEOMETRY_SHADER, SHADER("terrain.g.glsl"), */
    /*                  &terrain_geom); */
    /* 	assert(ok && "terrain geometry shader"); */

    /* ok = make_shader(GL_TESS_CONTROL_SHADER, SHADER("terrain.tc.glsl"), */
    /*                  &terrain_tc); */
    /* 	assert(ok && "terrain tessellation control shader"); */
    /* 	check_gl(); */

    /* ok = make_shader(GL_TESS_EVALUATION_SHADER, SHADER("terrain.te.glsl"), */
    /*                  &terrain_teval); */
    /* 	assert(ok && "terrain tessellation eval shader"); */
    /* 	check_gl(); */

	ok = make_shader(GL_FRAGMENT_SHADER, SHADER("test.f.glsl"), &fragment);
	assert(ok && "default fragment shader");
	check_gl();

	// camera
	mat4_perspective(90 /* fov */,
			 (float)width / height,
			 1,
			 5000,
			 resources->proj_persp);

	/* Shader program */
    /* struct shader *terrain_shaders[] = */
    /*     { &terrain_vertex, &fragment, &terrain_tc, &terrain_teval, */
    /*       &terrain_geom }; */

    /* struct shader *terrain_shaders[] = */
    /*     { &terrain_vertex, &fragment, &terrain_tc, &terrain_teval }; */

    /* struct shader *terrain_shaders[] = */
    /*     { &terrain_vertex, &fragment, &terrain_geom }; */

    struct shader *terrain_shaders[] =
        { &terrain_vertex, &fragment };

    ok = make_program_from_shaders(terrain_shaders, ARRAY_SIZE(terrain_shaders),
                                   &resources->programs[TERRAIN_PROGRAM]);

	assert(ok && "terrain program");
    check_gl();

	ok = make_program(&vertex, &fragment, &resources->programs[DEFAULT_PROGRAM]);
	assert(ok && "vertex-color program");
    check_gl();

    GLuint programs[] =
        { resources->programs[TERRAIN_PROGRAM].handle
        , resources->programs[DEFAULT_PROGRAM].handle
        };

    // uniforms shared between all shaders
    for (size_t i = 0; i < ARRAY_SIZE(programs); ++i) {
        GLuint handle = programs[i];

        // Program variables
        resources->uniforms.camera_position =
            glGetUniformLocation(handle, "camera_position");
        check_gl();

        /* resources->uniforms.depth_vp = */
        /*     glGetUniformLocation(handle, "depth_vp"); */
        /* check_gl(); */

        resources->uniforms.depth_mvp =
            glGetUniformLocation(handle, "depth_mvp");
        check_gl();

        resources->uniforms.light_intensity =
            glGetUniformLocation(handle, "light_intensity");
        check_gl();

        resources->uniforms.sky_intensity =
            glGetUniformLocation(handle, "sky_intensity");
        check_gl();

        resources->uniforms.time =
            glGetUniformLocation(handle, "time");
        check_gl();

        resources->uniforms.light_dir =
            glGetUniformLocation(handle, "light_dir");
        check_gl();

        resources->uniforms.sun_color =
            glGetUniformLocation(handle, "sun_color");
        check_gl();

        resources->uniforms.fog_on =
            glGetUniformLocation(handle, "fog_on");
        check_gl();

        /* resources->uniforms.diffuse_on = */
        /*     glGetUniformLocation(handle, "diffuse_on"); */
        /* check_gl(); */

        resources->uniforms.model =
            glGetUniformLocation(handle, "model");
        check_gl();

        resources->uniforms.mvp =
            glGetUniformLocation(handle, "mvp");
        check_gl();

        /* resources->uniforms.model_view = */
        /*     glGetUniformLocation(handle, "model_view"); */

        resources->uniforms.normal_matrix =
            glGetUniformLocation(handle, "normal_matrix");
        check_gl();

        resources->vertex_attrs[va_normal] =
            (gpu_addr)glGetAttribLocation(handle, "normal");
        check_gl();

        resources->vertex_attrs[va_position] =
            (gpu_addr)glGetAttribLocation(handle, "position");
        check_gl();

    }

    // TODO: auto-generate these somehow?
	resources->vertex_attrs[va_color] =
        (gpu_addr)glGetAttribLocation(resources->programs[DEFAULT_PROGRAM]
                                        .handle, "color");
    /* assert(resources->attributes.color != 0xFFFFFFFF); */
    check_gl();
}


static void
recalc_normals(GLint nm_uniform, mat4 *model_view, mat4 *normal) {
  mat4_inverse(model_view, normal);
  mat4_transpose(normal, normal);
  /* mat4_copy(model_view, normal); */
  glUniformMatrix4fv(nm_uniform, 1, 0, normal);
}

static void gamma_correct(float *c, float *d) {
    float gamma = 1.0/2.2;
    d[0] = powf(c[0], gamma);
    d[1] = powf(c[1], gamma);
    d[2] = powf(c[2], gamma);
}

void render (struct game *game, struct render_config *config) {
    float gtmp[3];
    struct resources *res = &game->test_resources;

	glEnable(GL_DEPTH_TEST);

    float sky_intensity = clamp(res->light_intensity, 0.2, 1.0);
    vec3_scale(res->sun_color, sky_intensity, gtmp);

    glClearColor( gtmp[0], gtmp[1], gtmp[2], 1.0 ); //clear background screen to black
    /* glClearColor( 0.5294f * adjust, 0.8078f * adjust, 0.9216f * adjust, 1.0f ); //clear background screen to black */
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    check_gl();

    static float id[MAT4_ELEMS] = { 0 };
    static float view[MAT4_ELEMS] = { 0 };
    static float view_proj[MAT4_ELEMS] = { 0 };
    static float normal_matrix[MAT4_ELEMS] = { 0 };
    static float model_view[MAT4_ELEMS] = { 0 };
    static float depth_mvp[MAT4_ELEMS] = { 0 };
    mat4_id(id);
    mat4_id(model_view);

    mat4 *mvp = res->test_mvp;
    mat4 *projection = config->projection;
    mat4 *light = res->light_dir;

    struct node *camera_node = get_node(&config->camera);
    assert(camera_node);

    const mat4 *camera = camera_node->mat;
    u32 num_entities;

    struct entity *entities =
        get_all_entities(&num_entities, NULL);

    struct gpu_program *current_program = NULL;

    struct gpu_program *terrain_program =
        &game->test_resources.programs[TERRAIN_PROGRAM];

    struct gpu_program *default_program =
        &game->test_resources.programs[DEFAULT_PROGRAM];

    /* mat4_multiply(view_proj, res->skybox.node.mat, mvp); */

    mat4_inverse((float*)camera, view);
    mat4_multiply(projection, view, view_proj);

    if (config->is_depth_pass) {
        glDisable(GL_CULL_FACE);
        mat4_multiply(bias_matrix, view_proj, config->depth_vp);
    }
    else {
        glCullFace(GL_BACK);
    }

    mat4_inverse((float *)camera, view);
    mat4_multiply(projection, view, view_proj);

    struct model *skybox_model = get_model(&res->skybox.model_id);
    assert(skybox_model);

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_model->texture);
    check_gl();

    for (u32 i = 0; i < num_entities; ++i) {
        struct entity *entity = &entities[i];

        if (entity->flags & ENT_INVISIBLE)
            continue;

        if (config->is_depth_pass && !entity->casts_shadows)
            continue;

        // TODO this is a bit wonky, refactor this
        current_program = i == 0 ? terrain_program : default_program;
        glUseProgram(current_program->handle);
        check_gl();


        glUniform3f(res->uniforms.camera_position,
                    camera[M_X],
                    camera[M_Y],
                    camera[M_Z]);

        glUniform1i(res->uniforms.fog_on, res->fog_on);
        check_gl();

        glUniform3f(res->uniforms.light_dir, light[0], light[1], light[2]);
        check_gl();

        glUniform1f(res->uniforms.light_intensity, res->light_intensity);
        check_gl();

        glUniform1f(res->uniforms.sky_intensity, sky_intensity);
        check_gl();

        glUniform3f(res->uniforms.sun_color,
                    res->sun_color[0],
                    res->sun_color[1],
                    res->sun_color[2]);
        check_gl();

        struct node *node = get_node(&entity->node_id);
        assert(node);
        if (node == NULL)
            return;

        mat4_multiply(view_proj, node->mat, mvp);
        mat4_copy(node->mat, model_view);
        mat4_multiply(config->depth_vp, model_view, depth_mvp);
        glUniformMatrix4fv(res->uniforms.depth_mvp, 1, 0, depth_mvp);
        check_gl();

        glUniformMatrix4fv(res->uniforms.mvp, 1, 0, mvp);
        check_gl();

        glUniformMatrix4fv(res->uniforms.model, 1, 0, node->mat);
        check_gl();

        recalc_normals(res->uniforms.normal_matrix, model_view, normal_matrix);
        check_gl();

        struct model *model = get_model(&entity->model_id);
        assert(model);
        struct geometry *geo = get_geometry(&model->geom_id);
        assert(geo);
        render_geometry(geo, res->vertex_attrs, current_program);
        check_gl();
    }

    if (!config->is_depth_pass) {
        mat4_inverse((float*)camera, view);
        mat4_remove_translations(view);
        mat4_multiply(projection, view, view_proj);

        render_skybox(&res->skybox, view_proj);
    }

    if (config->draw_ui)
        render_ui(&game->ui, view);

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
