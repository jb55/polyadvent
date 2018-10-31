
#include "entity.h"
#include "node.h"
#include "model.h"
#include <assert.h>

#define DEF_NUM_ENTITIES 1024

struct entity_id {
    u32 index;
    u32 generation;
};

void init_entity(struct entity *ent) {
    node_init(&ent->node);
    ent->casts_shadows = 0;
}

struct entity *get_entity(struct entity_system *system, struct entity_id id) {
    assert(id.generation <= system->generation);
    if (id.generation < system->generation)
        return NULL;
    return &system->entities[id.index];
}

void init_entity_system(struct entity_system *system) {
    system->generation = 0;
    system->entities = calloc(DEF_NUM_ENTITIES, sizeof(*system->entities));

    system->handles =
        calloc(DEF_NUM_ENTITIES, sizeof(*system->handles));
}


