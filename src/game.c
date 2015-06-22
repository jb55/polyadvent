
#include "mat4/mat4.h"
#include "game.h"

void init_game(struct game_state *game) {
  float *mvp = &game->test_resources.test_mvp[0];
  float *normal = &game->test_resources.normal_matrix[0];
  mat4_id(mvp);
  mat4_id(normal);
}
