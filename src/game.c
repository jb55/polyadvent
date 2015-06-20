
#include "mat4/mat4.h"
#include "game.h"

void init_game(struct game_state *game) {
  float *mvp = &game->test_resources.test_mvp[0];
  float v3[] = { 1, 1, 0 };
  mat4_id(mvp);
  mat4_rotate(mvp, 45, v3, mvp);
}
