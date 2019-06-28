
#include "entity.h"
#include "node.h"
#include "model.h"
#include "resource.h"
#include <assert.h>

#define DEF_NUM_ENTITIES 1024

static struct resource_manager esys;

struct entity *get_all_entities(u32 *count, entity_id **ids) {
    return get_all_resources(&esys, count, ids);
}

struct entity *init_entity(struct entity *ent) {
    node_init(&ent->node);
    ent->casts_shadows = 1;
    return ent;
}

struct entity *get_entity(entity_id *ent_id) {
    return get_resource(&esys, ent_id);
}

static inline struct entity *new_uninitialized_entity(entity_id *id) {
    return new_resource(&esys, id);
}

struct entity *new_entity(entity_id *id) {
    return init_entity(new_uninitialized_entity(id));
}

struct entity *add_entity(struct entity *e, entity_id *id) {
    struct entity *new = new_uninitialized_entity(id);
    *new = *e;
    return new;
}

void destroy_entities() {
    entity_id *ids;
    u32 count;
    get_all_resources(&esys, &count, &ids);
    for (u32 i = RESERVED_ENTITIES; i < count; i++) {
        destroy_entity(&ids[i]);
    }
};

void destroy_entity(entity_id *id) {
    struct entity *ent = get_entity(id);
    node_detach_from_parent(&ent->node);
    destroy_resource(&esys, id);
}

void destroy_entity_system() {
    destroy_resource_manager(&esys);
}

void init_entity_system() {
    init_resource_manager(&esys, sizeof(struct entity), DEF_NUM_ENTITIES,
                          MAX_ENTITIES);
}


