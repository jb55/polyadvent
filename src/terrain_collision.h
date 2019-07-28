
#ifndef TERRAIN_COLLISION_H
#define TERRAIN_COLLISION_H

#include "terrain.h"
#include "node.h"
#include "model.h"

struct tri *collide_terrain(struct terrain *terrain, float *pos, struct model *model, float *move);

#endif /* TERRAIN_COLLISION_H */
