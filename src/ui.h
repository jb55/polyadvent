
#ifndef POLYADVENT_UI_H
#define POLYADVENT_UI_H

#include "common.h"
#include "shader.h"
#include "geometry.h"

struct ui {
    struct gpu_program *shader;
    struct geometry quad;
    gpu_addr attrs[n_vertex_attrs];

    struct ui_uniforms {
        GLint mvp;
        GLint uipos;
        GLint uisize;
        GLint texture;
    } uniforms;

    float ortho[MAT4_ELEMS];
};

void create_ui(struct ui *ui, int width, int height,
               struct gpu_program *);

void resize_ui(struct ui *ui, int width, int height);
void render_ui(struct ui *ui, float *camera);


#endif /* POLYADVENT_UI_H */
