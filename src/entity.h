
#ifndef ENTITY_H
#define ENTITY_H

#include "node.h"
#include "model.h"
#include "id.h"
#include "util.h"
#include <assert.h>

#define MAX_ENTITIES 2048

enum entity_flags {
  ENT_IS_PLAYER = 1 << 0,
  ENT_INVISIBLE = 1 << 1,
};

struct entity {
    node_id node_id;
    model_id model_id;
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
const char *entity_label(struct entity *);
struct entity *get_all_entities(u32 *count, entity_id **ids);
struct entity *new_entity_(entity_id *);
struct entity *new_entity_with_node(entity_id *, node_id *);
struct entity *new_debug_entity(entity_id *, float *pos);
void destroy_entity_system();

static inline struct entity *static_entities()
{
    return (struct entity*)_internal_get_entity_system()->resources;
}

static inline struct entity *new_entity(entity_id *id)
{
    if (id)
        assert((int)id->index == -1 && "error: " __FILE__ ":" STRIZE(__LINE__) " missing init_id or already initialized");
    return new_entity_(id);
}

#endif /* ENTITY_H */
