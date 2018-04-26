

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "geometry.h"

struct perlin_settings {
  double freq;
  int depth;
  double amplitude;
  double exp;
};

struct terrain {
  struct geometry geom;
  struct perlin_settings settings;
  float width, height;
};

void terrain_init(struct terrain *terrain);
void terrain_create(struct terrain *terrain, struct perlin_settings *);
void terrain_destroy(struct terrain *terrain);


#endif /* POLYADVENT_TERRAIN_H */
