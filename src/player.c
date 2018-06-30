
#include "entity.h"
#include "vec3.h"


void
player_jump(struct entity *ent, float amt) {
  vec3_add(ent->acceleration, V3(0, 0, amt), ent->acceleration);
}
