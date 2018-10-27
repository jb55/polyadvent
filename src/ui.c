
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

void render_ui(struct ui *ui, float *view) {
    static float mvp[MAT4_ELEMS];
	glDisable(GL_DEPTH_TEST);
    glUseProgram(ui->shader.handle);
    check_gl();

    float uipos[2] = {0.01, 0.01};
    float uisize[2] = {0.2, 0.2};

    mat4_multiply(ui->ortho, view, mvp);

    // setup camera for shader
    /* glUniform2f(ui->uipos_uniform, uipos[0] * uisize[0], uipos[1] * uisize[1]); */
    glUniform2f(ui->uipos_uniform, uipos[0], uipos[1]);
    glUniform2f(ui->uisize_uniform, uisize[0], uisize[1]);

    glUniformMatrix4fv(ui->mvp_uniform, 1, 0, ui->ortho);
    check_gl();

    // render quad
    render_geometry(&ui->quad, &ui->attrs);
    check_gl();
}


void resize_ui(struct ui *ui, int width, int height) {
    float left = 0;
    float right = 1.0;
    float bottom = 1.0;
    float top = 0;
    float near = 0.0;
    float far = 1.0;

    printf("%d %d\n", width, height);

    mat4_ortho(left, right, bottom, top, near, far, ui->ortho);
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

    ui->mvp_uniform   = glGetUniformLocation(program, "mvp");
    ui->uipos_uniform = glGetUniformLocation(program, "uipos");
    ui->uisize_uniform = glGetUniformLocation(program, "uisize");

    /* ui->attrs.normal   = (gpu_addr)glGetAttribLocation(program, "normal"); */
    ui->attrs.position = (gpu_addr)glGetAttribLocation(program, "position");
    ui->attrs.color    = (gpu_addr)glGetAttribLocation(program, "color");

    check_gl();
    resize_ui(ui, width, height);
    check_gl();
}
