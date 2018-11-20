
#include "entity.h"
#include "node.h"
#include "model.h"
#include "resource.h"
#include <assert.h>

#define DEF_NUM_ENTITIES 1024

static struct resource_manager esys;

static u64 entity_uuids = 0;

struct entity *get_all_entities(u32 *count, entity_id_t **ids) {
    return (struct entity*)get_all_resources(count, ids);
}

struct entity *init_entity(struct entity *ent) {
    node_init(&ent->node);
    ent->casts_shadows = 1;
    return ent;
}

struct entity *get_entity_pure(struct entity_id id) {
    struct entity_id pure_id;
    pure_id = id;

    return get_entity(&pure_id);
}

struct entity *get_entity(struct entity_id *ent_id) {
    struct entity_id *new_id;
    struct id *id = &ent_id->id;

    // rollover is ok
    /* assert(->generation <= esys.generation); */
    if (id->generation != esys.generation) {
        // try to find uuid in new memory layout
        for (u32 i = 0; i < esys.entity_count; i++) {
            new_id = &esys.ids[i];
            if (new_id->id.uuid == id->uuid) {
                id->index = new_id->id.index;
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
        .id = (struct id) {
            .index      = esys.resource_count,
            .uuid       = entity_uuids++,
            .generation = esys.generation,
        }
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

void destroy_entities() {
    for (u32 i = RESERVED_ENTITIES; i < esys.entity_count; i++) {
        destroy_entity(&esys.entities[i]);
    }
    esys.entity_count = RESERVED_ENTITIES;
};

void destroy_entity(entity_id_t id) {
    node_detach_from_parent(&ent->node);
    destroy_resource(&esys, id);
}

void destroy_entity_system() {
    destroy_resource_manager(&esys);
}

void init_entity_system() {
    esys.generation = 0;
    esys.entities = calloc(DEF_NUM_ENTITIES, sizeof(*esys.entities));

    esys.ids =
        calloc(DEF_NUM_ENTITIES, sizeof(*esys.ids));
}


