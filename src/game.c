
#include "mat4/mat4.h"
#include "game.h"

void init_game(struct game_state *game) {
  float *mvp = &game->test_resources.test_mvp[0];
  float *normal = &game->test_resources.normal_matrix[0];
  mat4_id(mvp);
  mat4_id(normal);

  // move the camera a bit
  mat4_translate(mvp, -10.0f, -10.0f, -40.0f, mvp);
}
