
#ifndef RESOURCE_H
#define RESOURCE_H

#include "common.h"

struct resource_id {
    u64 uuid;
    u32 index;
    u32 generation;
};

struct resource_manager {
    void *resources;
    struct resource_id *ids;
    u32 resource_count;
    u32 generation;
};

void *get_all_resources(struct resource_manager *, u32 *count, struct resource_id **ids);
void destroy_resource(struct resource_manager *);
void destroy_resource_manager(struct resource_manager *);

#endif /* RESOURCE_H */
