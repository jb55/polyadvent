
#include "skybox.h"
#include "util.h"
#include "texture.h"

static float skybox_vertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    0.0f, -1.0f, -1.0f,
    0.0f, -1.0f, -1.0f,
    0.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,

    1.0f, -1.0f,  1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f, -1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,

    0.0f, -1.0f, -1.0f,
    0.0f, -1.0f,  1.0f,
    0.0f,  1.0f,  1.0f,
    0.0f,  1.0f,  1.0f,
    0.0f,  1.0f, -1.0f,
    0.0f, -1.0f, -1.0f,

    1.0f, -1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    0.0f,  1.0f,  1.0f,
    0.0f,  1.0f,  1.0f,
    0.0f, -1.0f,  1.0f,
    1.0f, -1.0f,  1.0f,

    1.0f,  1.0f, -1.0f,
    0.0f,  1.0f, -1.0f,
    0.0f,  1.0f,  1.0f,
    0.0f,  1.0f,  1.0f,
    1.0f,  1.0f,  1.0f,
    1.0f,  1.0f, -1.0f,

    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    0.0f, -1.0f, -1.0f,
    0.0f, -1.0f, -1.0f,
    1.0f, -1.0f,  1.0f,
    0.0f, -1.0f,  1.0f
};

void create_skybox(struct skybox *skybox) {
    struct shader vertex, frag;
    struct shader *shaders[] = {&vertex, &frag};

    init_model(&skybox->model);
    skybox->model.geom.vertices = skybox_vertices;
    skybox->model.geom.num_verts = ARRAY_SIZE(skybox_vertices);
    make_buffer_geometry(&skybox->model.geom);

    static const char *faces[6] = {
      CUBEMAP("ame_siege/siege_rt.tga"),
      CUBEMAP("ame_siege/siege_lf.tga"),
      CUBEMAP("ame_siege/siege_up.tga"),
      CUBEMAP("ame_siege/siege_dn.tga"),
      CUBEMAP("ame_siege/siege_ft.tga"),
      CUBEMAP("ame_siege/siege_bk.tga"),
    };
    skybox->model.texture = create_cubemap(faces);

    make_shader(GL_VERTEX_SHADER, SHADER("skybox.v.glsl"), &vertex);
    check_gl();
    make_shader(GL_FRAGMENT_SHADER, SHADER("skybox.f.glsl"), &frag);
    check_gl();

    make_program_from_shaders(shaders, 2, &skybox->program);
    check_gl();

    skybox->uniforms.mvp =
        glGetUniformLocation(skybox->program.handle, "mvp");

    skybox->attrs.position = (gpu_addr)
        glGetAttribLocation(skybox->program.handle, "position");
}


void render_skybox(struct skybox *skybox, mat4 *camera) {
    glDepthMask(GL_FALSE);
    check_gl();

    glUseProgram(skybox->program.handle);
    check_gl();

    glUniformMatrix4fv(skybox->uniforms.mvp, 1, 0, camera);
    check_gl();

    glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->model.texture);
    check_gl();

    render_geometry(&skybox->model.geom, &skybox->attrs, &skybox->program);
    check_gl();

    glDepthMask(GL_TRUE);
    check_gl();
}
