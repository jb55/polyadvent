
#include "mat4/mat4.h"
#include "game.h"

mat4 *cam_init = (float[16]){
  0.955761, 0.228018, -0.185425, 0.000000,
  -0.293528, 0.779583, -0.552437, 0.000000,
  0.018780, 0.580299, 0.802257, 0.000000,
  -26.434353, -33.475662, -21.457537, 1.000000
};

void game_init(struct game *game) {
  float *mvp = game->test_resources.test_mvp;
  float *normal = game->test_resources.normal_matrix;
  float *camera = game->test_resources.camera;
  float *light_dir = game->test_resources.light_dir;

  mat4_id(mvp);
  mat4_id(normal);
  mat4_id(camera);

  light_dir[0] = 1;
  light_dir[1] = 1;
  light_dir[2] = 0;

  // move the camera a bit
  /* mat4_translate(camera, 1.0f, 1.0f, 20.0f, camera); */
  mat4_copy(cam_init, camera);

  input_init(&game->input);
}
