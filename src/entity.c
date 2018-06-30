
#include "entity.h"
#include "vec3.h"

static u32 entity_ids = 0;

void entity_init(struct entity *entity) {
  entity->geometry      = NULL;
  entity->flags         = ENT_STATIC;
  entity->id            = entity_ids++;

  vec3_all(entity->acceleration, 0);
  vec3_all(entity->velocity, 0);

  node_init(&entity->node);
}
