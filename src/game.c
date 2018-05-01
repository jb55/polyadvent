
#include "mat_util.h"
#include "game.h"

mat4 *cam_init = (float[16]){
  0.955761, 0.228018, -0.185425, 0.000000,
  -0.293528, 0.779583, -0.552437, 0.000000,
  0.018780, 0.580299, 0.802257, 0.000000,
  -71.766136, -47.881512, -44.216671, 1.000000
};

void game_init(struct game *game) {
  mat4 *mvp = game->test_resources.test_mvp;
  struct node *root = &game->test_resources.root;
  struct node *camera = &game->test_resources.camera;
  struct node *player = &game->test_resources.player;
  struct node *player_camera = &game->test_resources.player_camera;
  struct node *terrain_node = &game->test_resources.terrain_node;
  mat4 *light_dir = game->test_resources.light_dir;

  mat4_id(mvp);

  light_dir[0] = 1;
  light_dir[1] = 1;
  light_dir[2] = 0.8;

  node_init(root);
  node_init(player);
  node_init(camera);
  node_init(player_camera);
  node_init(terrain_node);

  root->label = "root";
  player->label = "player";
  camera->label = "camera";
  player_camera->label = "player_camera";
  terrain_node->label = "terrain_node";

  node_attach(player, root);
  node_attach(player_camera, player);
  node_attach(camera, root);

  /* vec3_all(camera->scale, -1); */
  camera->mirrored = 1;

  node_translate(player, V3(10,10,0));
  node_translate(camera, V3(0,0,20));
  node_translate(player_camera, V3(10,10,20));
  /* node_recalc(camera); */

  /* player_camera->mirrored = 1; */
  /* camera->parent = player_camera; */
  quat_axis_angle(V3(1,0,0), 45, camera->orientation);

  // move the camera a bit
  /* mat4_translate(camera, 1.0f, 1.0f, 20.0f, camera); */
  /* mat4_copy(cam_init, camera); */
  /* mat4_id(camera_pos); */

  terrain_node->pos[2] = 20.0;

  /* mat4_scale(player, V3(0.36,0.36,PLAYER_HEIGHT), player); */

  input_init(&game->input);
}
