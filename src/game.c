
#include "mat4.h"
#include "vec3.h"
#include "game.h"

mat4 *cam_init = (float[16]){
  0.955761, 0.228018, -0.185425, 0.000000,
  -0.293528, 0.779583, -0.552437, 0.000000,
  0.018780, 0.580299, 0.802257, 0.000000,
  -71.766136, -47.881512, -44.216671, 1.000000
};

void game_init(struct game *game) {
  mat4 *mvp = game->test_resources.test_mvp;
  struct node *camera = &game->test_resources.camera;
  mat4 *terrain = game->test_resources.terrain_node;
  struct node *player = &game->test_resources.player;
  mat4 *light_dir = game->test_resources.light_dir;

  mat4_id(mvp);

  light_dir[0] = 1;
  light_dir[1] = 1;
  light_dir[2] = 0.8;

  node_init(player);
  node_init(camera);

  /* vec3_all(camera->scale, -1); */
  camera->mirrored = 1;

  node_translate(player, V3(10,10,0));
  node_translate(camera, V3(50,0,20));

  camera->rot[0] = -45;

  // move the camera a bit
  /* mat4_translate(camera, 1.0f, 1.0f, 20.0f, camera); */
  /* mat4_copy(cam_init, camera); */
  /* mat4_id(camera_pos); */

  mat4_id(terrain);

  terrain[M_Z] = 20.0;

  /* mat4_scale(player, V3(0.36,0.36,PLAYER_HEIGHT), player); */

  input_init(&game->input);
}
