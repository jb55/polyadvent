
#ifndef RESOURCE_H
#define RESOURCE_H

#include "common.h"

struct resource_id {
    u64 uuid;
    u32 index;
    u32 generation;
};

struct resource_manager {
    u8 *resources;
    struct resource_id *ids;
    u32 resource_count;
    u32 generation;
    u32 elem_size;
    u32 max_elements;
};

void *get_resource(struct resource_manager *r, struct resource_id *id);
void *get_all_resources(struct resource_manager *, u32 *count, struct resource_id **ids);
void destroy_resource(struct resource_manager *, struct resource_id *id);
void destroy_resource_manager(struct resource_manager *);
struct resource_id new_id(struct resource_manager *);
void *new_resource(struct resource_manager *, struct resource_id *id);

void init_resource_manager(struct resource_manager *r, u32 elem_size,
                           u32 initial_elements, u32 max_elements);

#endif /* RESOURCE_H */
