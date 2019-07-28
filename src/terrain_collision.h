
#ifndef TERRAIN_COLLISION_H
#define TERRAIN_COLLISION_H

#include "terrain.h"
#include "node.h"
#include "model.h"

void collide_terrain(struct terrain *terrain, struct node *node, struct model *model, float *move);

#endif /* TERRAIN_COLLISION_H */
