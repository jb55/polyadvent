
#include "mat_util.h"
#include "game.h"
#include "ply.h"
#include "model.h"
#include "terrain.h"
#include "render.h"
#include "util.h"
#include "update.h"
#include "entity.h"
#include <assert.h>

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

struct entity *get_player(struct resources *res) {
    struct entity *player = get_entity(&res->player_id);
    assert(player);
    return player;
}

void game_init(struct game *game, int width, int height) {
    init_gl(&game->test_resources, width, height);
    init_entity_system();
    check_gl();

    float pos[3];
    struct resources *res = &game->test_resources;
    mat4 *mvp = res->test_mvp;
    struct node *root = &res->root;
    struct node *camera = &res->camera;
    struct node *sun_camera = &res->sun_camera;
    struct terrain *terrain = &game->terrain;
    struct entity *player;

    mat4 *light_dir = res->light_dir;
    int ok = 0;

    const double size = 10000.0;

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

    create_ui(&game->ui, width, height, &res->programs[UI_PROGRAM]);

    check_gl();

    init_terrain(terrain, size);

    mat4_id(mvp);

    res->time = 0;
    res->light_intensity = 0.8;

    light_dir[0] = 0.8;
    light_dir[1] = 0.8;
    light_dir[2] = 0.8;

    res->sun_color[0] = 1.0;
    res->sun_color[1] = 0.9;
    res->sun_color[2] = 0.8;

    res->sun_color[0] = 0.5;
    res->sun_color[1] = 0.6;
    res->sun_color[2] = 0.7;

    game->test_resources.fog_on = 1;
    game->test_resources.diffuse_on = 0;

    res->orbit_camera.azimuth = RAD(90.0);
    res->orbit_camera.inclination = RAD(180.0);
    res->orbit_camera.radius = RAD(180.0);

    node_init(root);
    node_init(camera);
    node_init(sun_camera);

    // ENTITIES

    // player entity
    player = new_entity(&res->player_id);
    ok = load_model(&player->model, "pirate-officer");
    assert(ok);
    player->node.label = "player";
    node_attach(&player->node, root);
    node_translate(&player->node, V3(terrain->size/2.,terrain->size/2.,0.0));

    struct entity *tower = new_entity(NULL);
    ok = load_model(&tower->model, "tower");
    assert(ok);
    tower->node.label = "tower";
    node_attach(&tower->node, &player->node);
    node_translate(&tower->node, V3(0.0, 40.0, 0.0));


    // END ENTITIES


    // player init

    root->label = "root";
    camera->label = "camera";

    node_attach(camera, &player->node);

    quat_axis_angle(V3(1,0,0), -45, camera->orientation);

    node_rotate(camera, V3(100, 0, 0));
    node_translate(camera, V3(0,-40,20));

    input_init(&game->input);

    // FBO STUFF
    init_fbo(&res->shadow_buffer);
    resize_fbos(player, &res->shadow_buffer, res->proj_ortho, width, height);
    // FBO STUFF END
}
