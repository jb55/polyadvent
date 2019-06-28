
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "resource.h"
#include "debug.h"
#include "util.h"

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
        debug("id gen %d != res gen %d, refreshing\n", id->generation, r->generation);
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

/* static struct resource_id new_id(struct resource_manager *r) { */
/*     return (struct resource_id){ */
/*         .index      = r->resource_count, */
/*         .uuid       = ++resource_uuids, */
/*         .generation = r->generation, */
/*     }; */
/* } */

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
    new_mem = realloc(r->resources, new_size * r->elem_size);
    if (!new_mem) {
        // yikes, out of memory, bail
        assert(new_mem);
        return;
    }

    r->resources = new_mem;
    new_mem = realloc(r->ids, sizeof(struct resource_id) * new_size);

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

static int compact(struct resource_manager *r)
{
    int free_slot = -1;
    int fresh_elements = 0;
    u8 *dst;
    u8 *src = NULL;
    u8 * const start = r->resources;

    for (int i = 0; i < (int)r->resource_count; i++) {
        if (free_slot != -1 && r->ids[i].generation != 0) {
            debug("res: moving from %d to empty slot %d\n", i, free_slot);
            src = index_resource(r, i);
            dst = index_resource(r, free_slot);

            if (src == dst) {
                unusual("resource compact: trying to compact into the same location\n");
                continue;
            }

            debug("res: src %zu dst %zu count %d cap %d\n", src - start, dst - start,
                  r->resource_count, r->current_capacity);

            // move data to free slot, clear the old slot
            assert(dst + r->elem_size <= start + r->elem_size * r->current_capacity);
            memcpy(dst, src, r->elem_size);
            assert(src + r->elem_size <= start + r->elem_size * r->current_capacity);
            memset(src, 0, r->elem_size);

            // move ids to free slot, increasing generation
            r->ids[free_slot] = r->ids[i];
            r->ids[free_slot].generation++;
            r->ids[free_slot].index = free_slot;

            // clear the old data
            memset(&r->ids[i], 0, sizeof(*r->ids));
            free_slot = min(free_slot, i);
            fresh_elements++;
        }
        else if (r->ids[i].generation == 0) {
            debug("res: found deleted slot %d\n", i);
            free_slot = free_slot == -1 ? i : min(free_slot, i);
        }
        else {
            fresh_elements++;
        }

    }

    if ((int)r->resource_count != fresh_elements)
        debug("res: updated resource_count to %d\n", fresh_elements);
    r->resource_count = fresh_elements;

    return fresh_elements;
}

static void compact_or_resize(struct resource_manager *r)
{
    int orig = r->resource_count;
    int compacted = compact(r);
    debug("%d compacted to %d > %d?\n", orig, compacted, r->current_capacity);
    if (compacted > (int)r->current_capacity)
        resize(r);
}

void *new_resource(struct resource_manager *r, struct resource_id *id)
{
    struct resource_id *fresh_id;

    if (r->resource_count + 1 > r->max_capacity)
        return NULL;

    if (r->resource_count + 1 >= r->current_capacity)
        compact_or_resize(r);

    fresh_id = &r->ids[r->resource_count];

    new_id(r, fresh_id);

    if (id)
        *id = *fresh_id;

    return index_resource(r, r->resource_count++);
}

void *get_resource(struct resource_manager *r, struct resource_id *id) {
    refresh_id(r, id, id);

    return index_resource(r, id->index);
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
    r->ids[id->index].generation = 0;
    r->ids[id->index].index = -1;
    r->ids[id->index].uuid = -1;

    // clear the user's id as well, since it's no longer valid
    id->generation = 0;
    id->uuid = -1;
    id->index = -1;

    /* memmove(index_resource(r, id->index), */
    /*         index_resource(r, id->index+1), r->resource_count -) */
}
