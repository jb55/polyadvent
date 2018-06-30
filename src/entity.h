
#ifndef POLYADVENT_ENTITY_H
#define POLYADVENT_ENTITY_H

#include "common.h"
#include "geometry.h"
#include "node.h"

#define MAX_ENTITIES 1024

enum entity_flags {
  ENT_RIGID_BODY = 1 << 0,
  ENT_STATIC     = 1 << 1, // immovable
};

struct entity {
  u32 id;
  struct node node;
  struct geometry *geometry;
  float impulse[3];
  float acceleration[3];
  float velocity[3];
  int flags;
};

void
entity_init(struct entity *entity);

struct entity *
entity_get(int id);

#endif /* POLYADVENT_ENTITY_H */
