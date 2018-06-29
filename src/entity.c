
#include "entity.h"

static u32 entity_ids = 0;

void entity_init(struct entity *entity) {
  entity->geometry = NULL;
  entity->flags = ENT_STATIC;
  entity->id = entity_ids++;
  node_init(&entity->node);
}
