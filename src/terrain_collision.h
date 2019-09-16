
#ifndef TERRAIN_COLLISION_H
#define TERRAIN_COLLISION_H

#include "terrain.h"
#include "node.h"
#include "model.h"

struct tri *collide_terrain(struct terrain *terrain, float *pos, float *move, float *pen);

#endif /* TERRAIN_COLLISION_H */
