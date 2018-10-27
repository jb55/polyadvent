
#ifndef POLYADVENT_UI_H
#define POLYADVENT_UI_H

#include "common.h"
#include "shader.h"
#include "geometry.h"

struct ui {
    struct gpu_program shader;
    struct geometry quad;
    struct attributes attrs;
    GLint mvp_uniform;
    GLint uipos_uniform;
    GLint uisize_uniform;
    float ortho[MAT4_ELEMS];
};

void create_ui(struct ui *ui, int width, int height);
void resize_ui(struct ui *ui, int width, int height);
void render_ui(struct ui *ui, float *camera);


#endif /* POLYADVENT_UI_H */
