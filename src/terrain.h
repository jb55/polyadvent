

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "geometry.h"

struct terrain {
  struct geometry geom;
  float width, height;
};

void terrain_init(struct terrain *terrain);
void terrain_create(struct terrain *terrain);
void terrain_destroy(struct terrain *terrain);


#endif /* POLYADVENT_TERRAIN_H */
