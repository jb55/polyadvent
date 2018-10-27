
#include "ui.h"
#include "mat4.h"
#include "buffer.h"
#include "geometry.h"
#include "util.h"
#include "common.h"


//  v1------v0
//  |       |
//  |       |
//  |       |
//  v2------v3

static GLfloat quad_vertices[] = {
  0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,    // v0-v1-v2-v3 front
};


static GLfloat quad_normals[] = {
  0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, // front
};

static GLuint quad_indices[] = {0, 1, 2,  0, 2, 3};


static void create_quad(struct geometry *geom)
{
    init_geometry(geom);
    geom->indices = quad_indices;
    geom->vertices = quad_vertices;
    geom->colors = quad_normals;
    geom->num_indices = ARRAY_SIZE(quad_indices);
    geom->num_verts = ARRAY_SIZE(quad_vertices);
    make_buffer_geometry(geom);
    check_gl();
}

void render_ui(struct ui *ui) {
    glUseProgram(ui->shader.handle);
    check_gl();

    // setup camera for shader
    glUniformMatrix4fv(ui->view_uniform, 1, 0, ui->camera);
    check_gl();

    // render quad
    render_geometry(&ui->quad, &ui->attrs);
    check_gl();
}


void resize_ui(struct ui *ui, int width, int height) {
    float left = 0;
    float right = width;
    float top = 0;
    float bottom = height;
    float near = -100;
    float far = 100;

    mat4_ortho(left, right, bottom, top, near, far, ui->camera);
}


void create_ui(struct ui *ui, int width, int height) {
    struct shader vertex;
    struct shader fragment;
    int ok = 0;

    create_quad(&ui->quad);
    check_gl();

    ok = make_shader(GL_VERTEX_SHADER, SHADER("ui.v.glsl"), &vertex);
    assert(ok && "ui vertex shader");

    ok = make_shader(GL_FRAGMENT_SHADER, SHADER("ui.f.glsl"), &fragment);
    assert(ok && "ui fragment shader");

    ok = make_program(&vertex, &fragment, &ui->shader);
    assert(ok && "ui program");

    GLuint program = ui->shader.handle;

    ui->view_uniform = glGetUniformLocation(program, "view");

    /* ui->attrs.normal   = (gpu_addr)glGetAttribLocation(program, "normal"); */
    ui->attrs.position = (gpu_addr)glGetAttribLocation(program, "position");
    ui->attrs.color    = (gpu_addr)glGetAttribLocation(program, "color");

    check_gl();
    resize_ui(ui, width, height);
    check_gl();
}
