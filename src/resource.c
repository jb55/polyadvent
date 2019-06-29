
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "resource.h"
#include "debug.h"
#include "util.h"

static u64 resource_uuids = 0;

static inline void *index_resource(struct resource_manager *r, int i) {
    unsigned char *p = r->resources;
    return p + (i * r->elem_size);
}

void *get_all_resources(struct resource_manager *r, u32 *count, struct resource_id **ids) {
    if (count != 0)
        *count = r->resource_count;

    if (ids != 0)
        *ids = r->ids;

    return r->resources;
}

void init_resource_id(struct resource_id *id) {
    id->index = -1;
    id->generation = -1;
    id->uuid = -1;
}

void init_resource_manager(struct resource_manager *r, u32 elem_size,
                           u32 initial_elements, u32 max_elements) {
    r->generation = 1;
    r->resource_count = 0;
    r->elem_size = elem_size;
    r->max_capacity = max_elements;
    r->current_capacity = initial_elements;

    assert(initial_elements != 0);

    r->resources = calloc(initial_elements, elem_size);
    r->ids = calloc(initial_elements, sizeof(*r->ids));
}

void destroy_resource_manager(struct resource_manager *r) {
    free(r->ids);
    free(r->resources);
}

static int refresh_id(struct resource_manager *r, struct resource_id *id,
                      struct resource_id *new)
{
    // rollover is ok
    /* assert(->generation <= esys.generation); */
    if (id->generation != r->generation) {
        /* debug("id gen %d != res gen %d, refreshing\n", id->generation, r->generation); */
        // try to find uuid in new memory layout
        for (u32 i = 0; i < r->resource_count; i++) {
            struct resource_id *newer_id = &r->ids[i];
            if (newer_id->uuid == id->uuid) {
                /* debug("found %llu, ind %d -> %d\n", new_id->uuid, new_id->index, new->index); */
                new->index = newer_id->index;
                new->generation = r->generation;
                return REFRESHED_ID;
            }
        }

        // entity was deleted
        return RESOURCE_DELETED;
    }

    // doesn't need refreshed
    return REFRESH_NOT_NEEDED;
}
 
int is_resource_destroyed(struct resource_id *id) {
    return id->generation == 0;
}

static void new_id(struct resource_manager *r, struct resource_id *id)
{
    id->index = r->resource_count;
    id->uuid  = ++resource_uuids;
    id->generation = r->generation;
    assert(id->generation);
}

static void resize(struct resource_manager *r)
{
    void *new_mem;
    u32 new_size = r->resource_count * 1.5;
    if (new_size >= r->max_capacity)
        new_size = r->max_capacity;

    debug("resizing new_size %d\n", new_size);

    new_mem = realloc(r->resources, (new_size+1) * r->elem_size);
    if (!new_mem) {
        // yikes, out of memory, bail
        assert(new_mem);
        return;
    }

    r->resources = new_mem;
    new_mem = realloc(r->ids, sizeof(struct resource_id) * (new_size+1));

    if (!new_mem) {
        // yikes, out of memory, bail
        assert(new_mem);
        return;
    }
    r->current_capacity = new_size;
    r->ids = new_mem;

    // 2 to avoid issues with compact generation incrementing
    r->generation += 2;
}

void *new_resource(struct resource_manager *r, struct resource_id *id)
{
    struct resource_id *fresh_id;

    if (r->resource_count + 1 > r->max_capacity)
        return NULL;

    if (r->resource_count + 1 > r->current_capacity)
        resize(r);

    fresh_id = &r->ids[r->resource_count];

    new_id(r, fresh_id);

    if (id)
        *id = *fresh_id;

    return index_resource(r, r->resource_count++);
}

void *get_resource(struct resource_manager *r, struct resource_id *id) {
    if (id->generation == 0)
        return NULL;

    enum refresh_status res = refresh_id(r, id, id);

    if (res == RESOURCE_DELETED)
        return NULL;

    return index_resource(r, id->index);
}


void destroy_resource(struct resource_manager *r, struct resource_id *id) {
    if (is_resource_destroyed(id))
        return;

    enum refresh_status res = refresh_id(r, id, id);
    // entity already deleted
    debug("refresh res %d uuid %llu gen %d index %d\n", res,
          id->uuid, id->generation, id->index);

    if (res == RESOURCE_DELETED) {
        id->generation = 0;
        return;
    }

    r->resource_count--;
    r->generation++;

    memmove(index_resource(r, id->index),
            index_resource(r, id->index+1),
            r->elem_size * r->resource_count);

    memmove(&r->ids[id->index],
            &r->ids[id->index+1],
            sizeof(*r->ids) * r->resource_count);
}
