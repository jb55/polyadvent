
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
    node_id node_id;
    struct model *model;
    u32 flags;
    float velocity[3];
    float accel[3];
    int casts_shadows;
};

typedef struct resource_id entity_id;

struct entity *init_entity(struct entity *, node_id *id);
void destroy_entities();
struct resource_manager *_internal_get_entity_system();
void destroy_entity(entity_id *);
void init_entity_system();
struct entity *get_entity(entity_id *);
struct entity *get_all_entities(u32 *count, entity_id **ids);
struct entity *new_entity(entity_id *);
struct entity *new_entity_with_node(entity_id *, node_id *);
void destroy_entity_system();

#endif /* ENTITY_H */
