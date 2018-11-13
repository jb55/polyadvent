
#include "mat_util.h"
#include "game.h"
#include "ply.h"
#include "model.h"
#include "terrain.h"
#include "render.h"
#include "util.h"
#include "update.h"
#include "entity.h"
#include "texture.h"
#include "stb_image.h"
#include "skybox.h"
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

static void init_user_settings(struct user_settings *settings) {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    settings->mouse_sens = 0.1;
}

void game_init(struct game *game, int width, int height) {
    init_gl(&game->test_resources, width, height);
    init_entity_system();
    init_user_settings(&game->user_settings);
    check_gl();

    struct resources *res = &game->test_resources;
    mat4 *mvp = res->test_mvp;
    struct node *root = &res->root;
    struct node *camera = &res->camera.node;
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
    create_terrain(terrain, size);
    /* get_entity(&terrain->entity_id)->flags |= ENT_INVISIBLE; */

    create_skybox(&res->skybox, &res->programs[SKYBOX_PROGRAM]);
    /* node_translate(&res->skybox.node, V3(-100.0, -100.0, 0.0)); */
    /* node_scale(&res->skybox.node, size/4.0); */

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

    node_init(root);
    node_init(sun_camera);
    init_orbit(&res->camera);

    // ENTITIES

    // player entity
    player = new_entity(&res->player_id);
    ok = load_model(&player->model, "pirate-officer");
    assert(ok);
    player->node.label = "player";
    node_attach(&player->node, root);
    node_attach(&res->skybox.node, &player->node);
    /* node_attach(&res->camera.node, &player->node); */
    node_translate(&player->node, V3(terrain->size/2.,terrain->size/2.,0.0));

    res->camera.coords.azimuth = -quat_yaw(player->node.orientation) - RAD(90.0);
    res->camera.coords.inclination = RAD(60);
    res->camera.coords.radius = 5.0;

    struct entity *tower = new_entity(NULL);
    ok = load_model(&tower->model, "tower");
    assert(ok);
    tower->node.label = "tower";
    node_attach(&tower->node, &player->node);
    node_translate(&tower->node, V3(0.0, 50.0, 0.0));
    node_recalc(&tower->node);
    float z = terrain->fn(terrain, tower->node.mat[M_X], tower->node.mat[M_Y]);
    node_detach(&tower->node, &player->node);
    tower->node.mat[M_Z] = z;
    // END ENTITIES


    // player init

    root->label = "root";
    camera->label = "camera";

    input_init(&game->input);

    // FBO STUFF
    init_fbo(&res->shadow_buffer);
    resize_fbos(player, &res->shadow_buffer, res->proj_ortho, width, height);
    // FBO STUFF END

    // TEXTURES
    // END TEXTURES
}
