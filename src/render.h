#ifndef POLYADVENT_RENDER_H
#define POLYADVENT_RENDER_H

#include "geometry.h"

struct game;

struct render_config {
    int draw_ui;
    float *camera;
    float *projection;
};

void init_gl(struct resources *resources, int width, int height);
void render (struct game *game, struct render_config *config);

void wireframe_mode_on();
void wireframe_mode_off();

#endif /* POLYADVENT_RENDER_H */
