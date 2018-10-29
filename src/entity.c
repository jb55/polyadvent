
#include "entity.h"
#include "node.h"
#include "model.h"


void init_entity(struct entity *ent) {
    node_init(&ent->node);
    ent->casts_shadows = 0;
}
