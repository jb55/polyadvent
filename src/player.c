
#include "entity.h"
#include "vec3.h"


void
player_jump(struct entity *ent, float amt) {
  vec3_add(ent->impulse, V3(0, 0, amt), ent->impulse);
}
