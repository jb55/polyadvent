
#include "ui.h"
#include "mat4.h"
#include "buffer.h"
#include "geometry.h"
#include "util.h"


//  v1------v0
//  |       |
//  |       |
//  |       |
//  v2------v3

static const GLfloat quad_vertices[] = {
  0.5, 0.5, 0.5,  -0.5, 0.5, 0.5,  -0.5,-0.5, 0.5,   0.5,-0.5, 0.5,    // v0-v1-v2-v3 front
};


static const GLfloat quad_normals[] = {
  0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, // front
};

static const GLushort quad_indices[] = {0, 1, 2,  0, 2, 3};

static void
render_quad(struct geometry *geom, struct attributes *attrs) {
    bind_vbo(&geom->vbos.vertex, attrs->position);
    bind_vbo(&geom->vbos.normal, attrs->normal);

    bind_ibo(&geom->vbos.index);

    glDrawElements(
                   GL_TRIANGLES,
                   ARRAY_SIZE(quad_indices), /* count */
                   GL_UNSIGNED_SHORT,  /* type */
                   (void*)0            /* element array buffer offset */
                   );

    //glDisableVertexAttribArray(resources->attributes.position);
}


static void create_quad(struct geometry *geom)
{

}

void render_ui(struct ui *ui) {
    glUseProgram(ui->shader.handle);

    // render quad

    // setup camera for shader
}


void resize_ui(struct ui *ui, int width, int height) {
    float left = 0;
    float right = width;
    float top = 0;
    float bottom = height;
    float near = 0;
    float far = 2;

    mat4_ortho(left, right, bottom, top, near, far, ui->camera);
}


void create_ui(struct ui *ui, int width, int height) {
    struct shader vertex;
    struct shader fragment;

    make_shader(GL_VERTEX_SHADER, "ui.v.glsl", &vertex);
    make_shader(GL_FRAGMENT_SHADER, "ui.f.glsl", &vertex);
    make_program(&vertex, &fragment, &ui->shader);

    resize_ui(ui, width, height);
}
