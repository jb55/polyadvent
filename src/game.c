
#include "mat4/mat4.h"
#include "game.h"

mat4 *cam_init = (float[16]){
  1,    0,    0,     0,
  0,    0.67, -0.74, 0,
  0,    0.74, 0.66,  0,
  -0.1, -0.6, -3.0,  1
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
