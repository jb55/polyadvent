

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "entity.h"
#include "model.h"

struct point;

struct perlin_settings {
  double freq;
  int depth;
  double amplitude;
  double ox, oy;
  double o1, o1s;
  double o2, o2s;
  double scale;
  double exp;
};


struct terrain {
    struct entity_id entity_id;
    struct perlin_settings settings;
    struct point *samples;
    double (*fn)(struct terrain *, double, double);
    int n_samples;
    double size;
};

double old_noisy_boi(struct terrain *, double x, double y);

void init_terrain(struct terrain *terrain, float size);
void reset_terrain(struct terrain *terrain, float size);
void create_terrain(struct terrain *terrain);
void destroy_terrain(struct terrain *terrain);


#endif /* POLYADVENT_TERRAIN_H */
