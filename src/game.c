
#include "mat_util.h"
#include "game.h"
#include "terrain.h"

mat4 *cam_init = (float[16]){
  0.955761, 0.228018, -0.185425, 0.000000,
  -0.293528, 0.779583, -0.552437, 0.000000,
  0.018780, 0.580299, 0.802257, 0.000000,
  -71.766136, -47.881512, -44.216671, 1.000000
};

static void camera_update(struct node *node) {
  mat4 *persp = (float*)node->custom_update_data;
  mat4 *mat = (float*)node->mat;

  mat4_inverse(mat, mat);
  mat4_multiply(persp, mat, mat);
}

void game_init(struct game *game) {
  mat4 *mvp = game->test_resources.test_mvp;
  struct node *root = &game->test_resources.root;
  struct node *camera = &game->test_resources.camera;
  struct node *player = &game->test_resources.player;
  struct node *terrain_node = &game->test_resources.terrain_node;
  struct terrain *terrain = &game->terrain;
  mat4 *light_dir = game->test_resources.light_dir;

  const double size = 20000;
  const double pdist = 1.7;

  terrain->settings = (struct perlin_settings){
    .depth = 1,
    .freq  = 0.02,
    .o1 = 2.0, .o1s = 0.5,
    .o2 = 4.0, .o2s = 0.25,
    .amplitude  = 1.0,
    .ox = 0,
    .oy = 0,
    .exp = 5.3,
    .scale = 1.0
  };

  terrain_init(terrain);
  terrain->size = size;


  mat4_id(mvp);

  light_dir[0] = 0.8;
  light_dir[1] = 0.8;
  light_dir[2] = 0.8;

  game->test_resources.fog_on = 1;
  game->test_resources.diffuse_on = 0;

  node_init(root);
  node_init(player);
  node_init(camera);
  node_init(terrain_node);

  root->label = "root";
  player->label = "player";
  camera->label = "camera";
  terrain_node->label = "terrain_node";

  node_attach(player, root);
  node_attach(camera, player);

  quat_axis_angle(V3(1,0,0), -45, camera->orientation);

  /* camera->custom_update = camera_update; */
  /* camera->custom_update_data = (void*)game->test_resources.camera_persp; */

  /* vec3_all(camera->scale, -1); */
  /* camera->mirrored = 1; */

  node_translate(player, V3(terrain->size/2.,terrain->size/2.,0));
  node_translate(camera, V3(0,-30,100));
  /* node_recalc(camera); */

  terrain_node->pos[2] = 20.0;

  input_init(&game->input);
}
