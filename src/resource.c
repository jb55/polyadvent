
#include "resource.h"
#include <stdlib.h>
#include <string.h>

static u64 resource_uuids = 0;

static inline void *index_resource(struct resource_manager *r, int i) {
    return &r->resources[i * r->elem_size];
}

void *get_all_resources(struct resource_manager *r, u32 *count, struct resource_id **ids) {
    if (count != 0)
        *count = r->resource_count;
    if (ids != 0)
        *ids = r->ids;
    return r->resources;
}

void init_resource_manager(struct resource_manager *r, u32 elem_size,
                           u32 initial_elements, u32 max_elements) {
    r->generation = 1;
    r->resource_count = 0;
    r->elem_size = elem_size;
    r->max_elements = max_elements;

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
        // try to find uuid in new memory layout
        for (u32 i = 0; i < r->resource_count; i++) {
            struct resource_id *new_id = &r->ids[i];
            if (new_id->uuid == id->uuid) {
                new->index = new_id->index;
                new->generation = r->generation;
                return 1;
            }
        }

        // entity was deleted
        return 0;
    }

    // doesn't need refreshed
    return 2;
}

int is_resource_destroyed(struct resource_id *id) {
    return id->generation == 0;
}

void destroy_resource(struct resource_manager *r, struct resource_id *id) {
    if (is_resource_destroyed(id))
        return;

    int res = refresh_id(r, id, id);
    // entity already deleted
    if (res == 0) {
        id->generation = 0;
        return;
    }
    // generation 0 means destroyed
    id->generation = 0;

    memmove(index_resource(r, id->index),
            index_resource(r, id->index+1), r->resource_count -)
}

struct resource_id new_id(struct resource_manager *r) {
    return (struct resource_id){
        .index      = r->resource_count,
        .uuid       = resource_uuids++,
        .generation = r->generation,
    };
}

void *new_resource(struct resource_manager *r, struct resource_id *id) {
    struct resource_id fresh_id = new_id(r);

    if (id)
        *id = fresh_id;

    return index_resource(r, r->resource_count++);
}

void *get_resource(struct resource_manager *r, struct resource_id *id) {
    refresh_id(r, id, id);

    return index_resource(r, id->index);
}
