

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "geometry.h"

struct point;

struct perlin_settings {
  double freq;
  int depth;
  double amplitude;
  double ox, oy;
  double o1, o1s;
  double o2, o2s;
  double exp;
};

struct terrain {
  struct geometry geom;
  struct perlin_settings settings;
  struct point *samples;
  int n_samples;
  double size;
};

void terrain_init(struct terrain *terrain);
void terrain_create(struct terrain *terrain, struct perlin_settings *);
void terrain_destroy(struct terrain *terrain);


#endif /* POLYADVENT_TERRAIN_H */
