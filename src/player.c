
#include "node.h"
#include "vec3.h"
#include "forces.h"

void
player_jump(struct node *player) {
  force_apply(player, V3(0, 0, 0.5));
}
