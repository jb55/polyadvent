
#include "resource.h"

void *get_all_resources(struct resource_manager *r, u32 *count, struct resource_id **ids) {
    if (count != 0)
        *count = r->resource_count;
    if (ids != 0)
        *ids = r->ids;
    return r->resources;
}

void destroy_resource(struct resource_manager *r) {
   
}
