
#include "mat4/mat4.h"
#include "game.h"

mat4 *cam_init = (float[16]){
  0.952107, 0.186872, -0.241083, 0.000000,
  -0.302599, 0.688952, -0.656898, 0.000000,
  0.043785, 0.697792, 0.702532, 0.000000,
  -26.815081, -19.055046, -8.465672, 1.000000
};

void game_init(struct game *game) {
  float *mvp = game->test_resources.test_mvp;
  float *normal = game->test_resources.normal_matrix;
  float *camera = game->test_resources.camera;
  mat4_id(mvp);
  mat4_id(normal);
  mat4_id(camera);

  // move the camera a bit
  /* mat4_translate(camera, 1.0f, 1.0f, 20.0f, camera); */
  mat4_copy(cam_init, camera);

  input_init(&game->input);
}
