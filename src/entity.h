
#ifndef ENTITY_H
#define ENTITY_H

#include "node.h"
#include "model.h"

enum entity_flags {
  ENT_IS_PLAYER = 1 << 0,
  ENT_INVISIBLE = 1 << 1,
};

struct entity {
    struct node node;
    struct model model;
    u32 flags;
    float velocity[3];
    float accel[3];
    int casts_shadows;
};

struct entity_id {
    u64 uuid;
    u32 index;
    u32 generation;
};

struct entity *init_entity(struct entity *);
struct entity *delete_entities();
void init_entity_system();
struct entity *get_entity(struct entity_id *);
struct entity *get_all_entities(u32 *count, struct entity_id **ids);
struct entity *new_entity(struct entity_id *id);


#endif /* ENTITY_H */
