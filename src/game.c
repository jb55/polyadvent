
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

void game_init(struct game *game, int width, int height) {
    init_gl(&game->test_resources, width, height);
    check_gl();

    struct resources *res = &game->test_resources;
    mat4 *mvp = res->test_mvp;
    struct node *root = &res->root;
    struct node *camera = &res->camera;
    struct node *sun_camera = &res->sun_camera;
    struct entity *player = &res->player;
    struct terrain *terrain = &game->terrain;

    mat4 *light_dir = res->light_dir;
    int ok = 0;

    const double size = 10000;

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

    terrain_init(terrain);
    terrain->entity.model.shading = SHADING_TERRAIN;
    terrain->size = size;

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

    /* vec3(0.5,0.6,0.7); */

    // BRB: shadow mapping next!

    // FBO STUFF
    init_fbo(&res->shadow_buffer);
    resize_fbos(game, width, height);

    // FBO STUFF END

    game->test_resources.fog_on = 1;
    game->test_resources.diffuse_on = 0;

    res->orbit_camera.azimuth = RAD(90.0);
    res->orbit_camera.inclination = RAD(180.0);
    res->orbit_camera.radius = RAD(180.0);

    node_init(root);
    node_init(camera);
    node_init(sun_camera);

    init_entity(&terrain->entity);
    init_entity(player);
    player->casts_shadows = 1;
    terrain->entity.casts_shadows = 0;

    // player init
    ok = load_model(&player->model, "tower");
    assert(ok);
    player->model.shading = SHADING_VERT_COLOR;
    player->node.label = "player";

    root->label = "root";
    camera->label = "camera";
    terrain->entity.node.label = "terrain_node";

    node_attach(&player->node, root);
    node_attach(camera, &player->node);

    quat_axis_angle(V3(1,0,0), -45, camera->orientation);

    node_translate(&player->node, V3(terrain->size/2.,terrain->size/2.,0.0));

    node_rotate(camera, V3(100, 0, 0));
    node_translate(camera, V3(0,-40,20));

    terrain->entity.node.pos[2] = 20.0;

    input_init(&game->input);
}
