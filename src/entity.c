
#include "entity.h"
#include "node.h"
#include "model.h"
#include <assert.h>

#define DEF_NUM_ENTITIES 1024

struct entity_system {
    struct entity *entities;
    struct entity_id *ids;
    u32 entity_count;
    u32 generation;
};

static u64 entity_uuids = 0;

static struct entity_system esys;

struct entity *get_all_entities(u32 *count, struct entity_id **ids) {
    if (count != NULL)
        *count = esys.entity_count;
    if (ids != NULL)
        *ids = esys.ids;
    return esys.entities;
}

struct entity *init_entity(struct entity *ent) {
    node_init(&ent->node);
    init_model(&ent->model);
    ent->casts_shadows = 1;
    return ent;
}

struct entity *get_entity_pure(struct entity_id id) {
    struct entity_id pure_id;
    pure_id = id;

    return get_entity(&pure_id);
}

struct entity *get_entity(struct entity_id *id) {
    struct entity_id *new_id;
    // rollover is ok
    /* assert(->generation <= esys.generation); */
    if (id->generation != esys.generation) {
        // try to find uuid in new memory layout
        for (u32 i = 0; i < esys.entity_count; i++) {
            new_id = &esys.ids[i];
            if (new_id->uuid == id->uuid) {
                id->index = new_id->index;
                id->generation = esys.generation;
                return &esys.entities[id->index];
            }
        }

        // entity was deleted
        return NULL;
    }
    return &esys.entities[id->index];
}

static inline struct entity_id new_id() {
    return (struct entity_id){
        .index = esys.entity_count,
        .uuid  = entity_uuids++,
        .generation = esys.generation,
    };
}

static inline struct entity *new_uninitialized_entity(struct entity_id *id) {
    struct entity_id fresh_id;
    fresh_id = new_id();

    if (id)
        *id = fresh_id;

    return &esys.entities[esys.entity_count++];
}

struct entity *new_entity(struct entity_id *id) {
    return init_entity(new_uninitialized_entity(id));
}

struct entity *add_entity(struct entity *e, struct entity_id *id) {
    struct entity *new = new_uninitialized_entity(id);
    *new = *e;
    return new;
}

void destroy_entity(struct entity *ent) {

}

void destroy_entity_system() {
    free(esys.entities);
}

void init_entity_system() {
    esys.generation = 0;
    esys.entities = calloc(DEF_NUM_ENTITIES, sizeof(*esys.entities));

    esys.ids =
        calloc(DEF_NUM_ENTITIES, sizeof(*esys.ids));
}


