
#include "mat4/mat4.h"
#include "game.h"

mat4 *cam_init = (float[16]){
  1.000000,  0.000000,  0.000000, 0.000000,
  0.000000,  0.674875, -0.737931, 0.000000,
  0.000000,  0.737931,  0.674875, 0.000000,
  -0.100000,-0.131862, -6.496247, 1.000000
};

void game_init(struct game *game) {
  float *mvp = game->test_resources.test_mvp;
  float *normal = game->test_resources.normal_matrix;
  float *camera = game->test_resources.camera;
  mat4_id(mvp);
  mat4_id(normal);
  mat4_id(camera);

  // move the camera a bit
  /* mat4_translate(camera, -1.0f, -1.0f, -7.0f, camera); */
  mat4_copy(cam_init, camera);

  input_init(&game->input);
}
