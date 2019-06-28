
#ifndef ENTITY_H
#define ENTITY_H

#include "node.h"
#include "model.h"
#include "id.h"

#define RESERVED_ENTITIES 2
#define MAX_ENTITIES 2048

enum entity_flags {
  ENT_IS_PLAYER = 1 << 0,
  ENT_INVISIBLE = 1 << 1,
};

struct entity {
    struct node node;
    struct model *model;
    u32 flags;
    float velocity[3];
    float accel[3];
    int casts_shadows;
};

typedef struct resource_id entity_id;

struct entity *init_entity(struct entity *);
void destroy_entities();
void destroy_entity(entity_id *);
void init_entity_system();
struct entity *get_entity(entity_id *);
struct entity *get_all_entities(u32 *count, entity_id **ids);
struct entity *new_entity(entity_id *id);


#endif /* ENTITY_H */
