
#include "skybox.h"
#include "util.h"
#include "texture.h"

static GLfloat skybox_vertices[] = {
  1.0, 1.0, 1.0,  -1.0, 1.0, 1.0,  -1.0,-1.0, 1.0,   1.0,-1.0, 1.0,    // v0-v1-v2-v3 top
  1.0, 1.0,-1.0,   1.0, 1.0, 1.0,   1.0,-1.0, 1.0,   1.0,-1.0,-1.0,    // v5-v0-v3-v4 right
  -1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0,-1.0,  -1.0, 1.0,-1.0,   // v1-v0-v5-v6 front
  -1.0, 1.0, 1.0,  -1.0, 1.0,-1.0,  -1.0,-1.0,-1.0,  -1.0,-1.0, 1.0,   // v1-v6-v7-v2 left
  1.0,-1.0, 1.0,  -1.0,-1.0, 1.0,  -1.0,-1.0,-1.0,   1.0,-1.0,-1.0,    // v3-v2-v7-v4 back
  -1.0, 1.0,-1.0,   1.0, 1.0,-1.0,   1.0,-1.0,-1.0,  -1.0,-1.0,-1.0     // v4-v7-v6-v5 bottom
};


static GLfloat skybox_uvs[] = {
  1.0, 1.0, 1.0,  -1.0, 1.0, 1.0,  -1.0,-1.0, 1.0,   1.0,-1.0, 1.0,
  1.0, 1.0,-1.0,   1.0, 1.0, 1.0,   1.0,-1.0, 1.0,   1.0,-1.0,-1.0,
  -1.0, 1.0, 1.0,   1.0, 1.0, 1.0,   1.0, 1.0,-1.0,  -1.0, 1.0,-1.0,
  -1.0, 1.0, 1.0,  -1.0, 1.0,-1.0,  -1.0,-1.0,-1.0,  -1.0,-1.0, 1.0,
  1.0,-1.0, 1.0,  -1.0,-1.0, 1.0,  -1.0,-1.0,-1.0,   1.0,-1.0,-1.0,
  -1.0, 1.0,-1.0,   1.0, 1.0,-1.0,   1.0,-1.0,-1.0,  -1.0,-1.0,-1.0
};


static u32 skybox_indices[] = {
  0, 1, 2,   0, 2, 3,    // top     (+z)
  4, 5, 6,   4, 6, 7,    // right   (+x)
  8, 9,10,   8, 10,11,   // front (+y)
  12,13,14,  12,14,15,   // left    (-x)
  16,17,18,  16,18,19,   // back    (-y)
  20,21,22,  20,22,23    // bottom  (-z)
};


void create_skybox(struct skybox *skybox, struct gpu_program *program) {
    struct shader vertex, frag;
    struct shader *shaders[] = {&vertex, &frag};
    int ok;

    node_init(&skybox->node);
    init_model(&skybox->model);

    skybox->program = program;

    struct make_geometry mkgeom;
    init_make_geometry(&mkgeom);

    mkgeom.vertices = skybox_vertices;
    mkgeom.indices = skybox_indices;
    /* geom->tex_coords = skybox_uvs; */

    /* geom->num_uv_components = 3; */
    mkgeom.num_verts = ARRAY_SIZE(skybox_vertices);
    mkgeom.num_indices = ARRAY_SIZE(skybox_indices);

    make_buffer_geometry(&mkgeom, &skybox->model.geom_id);

    static const char *faces[6] = {
      CUBEMAP("hw_sahara/sahara_rt_flip.tga"),
      CUBEMAP("hw_sahara/sahara_lf_flip.tga"),
      CUBEMAP("hw_sahara/sahara_ft_flip.tga"),
      CUBEMAP("hw_sahara/sahara_bk.tga"),
      CUBEMAP("hw_sahara/sahara_up_flip.tga"),
      CUBEMAP("hw_sahara/sahara_dn_flip.tga"),
    };


    /* static const char *faces[6] = { */
    /*   CUBEMAP("ame_siege/siege_rt_flip.tga"), */
    /*   CUBEMAP("ame_siege/siege_lf_flip.tga"), */
    /*   CUBEMAP("ame_siege/siege_ft_flip.tga"), */
    /*   CUBEMAP("ame_siege/siege_bk.tga"), */
    /*   CUBEMAP("ame_siege/siege_up_flip.tga"), */
    /*   CUBEMAP("ame_siege/siege_dn_flip.tga"), */
    /* }; */

    skybox->model.texture = create_cubemap(faces);

    make_shader(GL_VERTEX_SHADER, SHADER("skybox.v.glsl"), &vertex);
    check_gl();
    make_shader(GL_FRAGMENT_SHADER, SHADER("skybox.f.glsl"), &frag);
    check_gl();

    ok = make_program_from_shaders(shaders, 2, skybox->program);
    assert(ok);
    check_gl();

    skybox->uniforms.mvp =
        glGetUniformLocation(skybox->program->handle, "mvp");

    skybox->attrs[va_position] = (gpu_addr)
        glGetAttribLocation(skybox->program->handle, "position");

    skybox->attrs[va_tex_coord] = (gpu_addr)
        glGetAttribLocation(skybox->program->handle, "tex_coord");
}


void render_skybox(struct skybox *skybox, mat4 *camera) {
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);
    check_gl();

    glUseProgram(skybox->program->handle);
    check_gl();

    glUniformMatrix4fv(skybox->uniforms.mvp, 1, 0, camera);
    check_gl();

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->model.texture);
    check_gl();

    render_geometry(get_geometry(&skybox->model.geom_id), skybox->attrs, skybox->program);
    check_gl();

    glDepthMask(GL_TRUE);
    check_gl();
    glDepthFunc(GL_LESS);
}
