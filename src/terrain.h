

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "geometry.h"

#define MAX_OCTAVES 6

struct point;

struct perlin_settings {
  double freq;
  int depth;
  double amplitude;
  double ox, oy;
  double n_octaves;
  double exp;
};


struct terrain {
  struct geometry geom;
  struct perlin_settings settings;
  struct point *samples;
  double (*fn)(struct terrain *, double, double);
  int n_samples;
  double size;
};

double old_noisy_boi(struct terrain *, double x, double y);

void terrain_init(struct terrain *terrain);
void terrain_create(struct terrain *terrain);
void terrain_destroy(struct terrain *terrain);


#endif /* POLYADVENT_TERRAIN_H */
