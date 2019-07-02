
#include "entity.h"
#include "node.h"
#include "model.h"
#include "resource.h"
#include "debug.h"
#include <assert.h>

#define DEF_NUM_ENTITIES 1024

static struct resource_manager esys;

struct resource_manager *_internal_get_entity_system()
{
    return &esys;
}

struct entity *get_all_entities(u32 *count, entity_id **ids) {
    return get_all_resources(&esys, count, ids);
}

struct entity *init_entity(struct entity *ent, node_id *id) {
    node_id new_id;
    init_model_id(&ent->model_id);
    if (id == NULL) {
        init_id(&new_id);
        new_node(&new_id);
        ent->node_id = new_id;
        /* debug("init_entity with new node_id %llu\n", new_id.uuid); */
    }
    else {
        ent->node_id = *id;
    }
    ent->casts_shadows = 1;
    return ent;
}

struct entity *get_entity(entity_id *ent_id) {
    return get_resource(&esys, ent_id);
}

static inline struct entity *new_uninitialized_entity(entity_id *id) {
    return new_resource(&esys, id);
}

struct entity *new_entity_(entity_id *id) {
    return new_entity_with_node(id, NULL);
}


struct entity *new_entity_with_node(entity_id *id, node_id *node)
{
    entity_id new_id;

    if (id == NULL) {
        id = &new_id;
        init_id(&new_id);
    }

    return init_entity(new_uninitialized_entity(id), node);
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

void destroy_entity(entity_id *id)
{
    struct entity *ent = get_entity(id);
    assert(ent);
    if (ent == NULL)
        return;

    struct node *node = get_node(&ent->node_id);
    assert(node);
    if (node == NULL)
        return;

    if (id->index < RESERVED_ENTITIES) {
        unusual("trying to destroy reserved entity with node %s\n", node->label);
        return;
    }

    node_detach_from_parent(node);
    destroy_resource(&esys, id);
}

const char *entity_label(struct entity *ent)
{
    struct node *node = get_node(&ent->node_id); assert(node);
    return node->label;
}

void destroy_entity_system() {
    destroy_resource_manager(&esys);
}

void init_entity_system() {
    init_resource_manager(&esys, sizeof(struct entity), DEF_NUM_ENTITIES,
                          MAX_ENTITIES);
}


