#ifndef POLYADVENT_RENDER_H
#define POLYADVENT_RENDER_H

#include "geometry.h"

void init_gl(struct resources *resources, int width, int height);
void render (struct resources *resources, struct geometry *geom);

void wireframe_mode_on();
void wireframe_mode_off();

#endif /* POLYADVENT_RENDER_H */
