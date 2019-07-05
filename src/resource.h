
#ifndef RESOURCE_H
#define RESOURCE_H

#include "common.h"

#define STATIC_UUID 0x5A51C00000000000

enum refresh_status {
    RESOURCE_DELETED,
    REFRESH_NOT_NEEDED,
    REFRESHED_ID
};

struct resource_id {
    u64 uuid;
    u32 index;
    u32 generation;
};

struct static_resources {
    u8 *resources;
    u32 count;
    u32 capacity;
};

struct resource_manager {
    u8 *resources;
    struct static_resources static_res;
    struct resource_id *ids;
    u32 resource_count;
    u32 generation;
    u32 elem_size;
    u32 slots_used;
    u32 max_capacity;
    u32 current_capacity;

    const char *name;
};

#define ideq(a, b) ((a)->uuid == (b)->uuid)

void init_id(struct resource_id *id);
void *get_resource(struct resource_manager *r, struct resource_id *id);
void *get_all_resources(struct resource_manager *, u32 *count, struct resource_id **ids);
void destroy_resource(struct resource_manager *, struct resource_id *id);
void destroy_resource_manager(struct resource_manager *);
void *new_resource(struct resource_manager *, struct resource_id *id);
void *new_static_resource(struct resource_manager *, struct resource_id *id);
void print_id(struct resource_id *, int nl);
void null_id(struct resource_id *id);
/* int is_static_resource(struct resource_id *id); */

void init_resource_manager(struct resource_manager *r, u32 elem_size,
                           u32 initial_elements, u32 max_elements, const char *name,
                           struct static_resources *static_res
                           );


static inline struct resource_id make_static_id(u32 index)
{
    return (struct resource_id){
        .uuid = STATIC_UUID,
        .index = index,
        .generation = -1
    };
}

static inline int is_static_resource(struct resource_id *id)
{
    return id->uuid == STATIC_UUID;
}


static inline int is_id_allocated(struct resource_id *id)
{
    return (int)id->index != -1;
}

#endif /* RESOURCE_H */
