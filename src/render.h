#ifndef POLYADVENT_RENDER_H
#define POLYADVENT_RENDER_H

#include "geometry.h"

void init_gl(struct test_resources * resources);
void render (struct test_resources * resources, struct geom *geom);

void wireframe_mode_on();
void wireframe_mode_off();

#endif /* POLYADVENT_RENDER_H */
