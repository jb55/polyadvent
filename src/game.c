
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
#include "quickhull.h"
#include "procmesh.h"
#include "util.h"

#include <assert.h>

mat4 *cam_init = (float[16]){
  0.955761, 0.228018, -0.185425, 0.000000,
  -0.293528, 0.779583, -0.552437, 0.000000,
  0.018780, 0.580299, 0.802257, 0.000000,
  -71.766136, -47.881512, -44.216671, 1.000000
};

int was_key_pressed_this_frame(struct game *game, int scancode) {
    return is_key_down_on_frame(&game->input, scancode, game->frame);
}

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

struct entity *get_terrain_entity(struct terrain *t) {
    struct entity *ent = get_entity(&t->entity_id);
    assert(ent);
    return ent;
}


static void init_user_settings(struct user_settings *settings) {
    SDL_SetRelativeMouseMode(SDL_TRUE);
    settings->mouse_sens = 0.1;
}


static void init_sdl(SDL_Window **window, int width, int height)
{
    SDL_Init( SDL_INIT_VIDEO );

    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); */
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    *window = SDL_CreateWindow("SDL2/OpenGL Demo", 0, 0, width, height,
                                          SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    SDL_GL_CreateContext(*window);
}

void quit_game(struct game *game)
{
    game->quit = 1;
}

void game_init(struct game *game, int width, int height) {
    init_sdl(&game->window, width, height);
    init_gl(&game->test_resources, width, height);
    init_entity_system();
    init_model_manager();
    init_node_manager();
    init_geometry_manager();
    init_user_settings(&game->user_settings);
    check_gl();

    struct resources *res = &game->test_resources;
    mat4 *mvp = res->test_mvp;

    init_id(&res->root_id);
    init_id(&res->sun_camera_id);
    init_id(&res->free_camera_id);

    struct node *root = new_node(&res->root_id);
    struct node *sun_camera = new_node(&res->sun_camera_id);
    struct terrain *terrain = &game->terrain;
    struct entity *player;

    assert(root->parent_id.generation == 0);
    assert(root);
    assert(sun_camera);

    mat4 *light_dir = res->light_dir;
    int ok = 0;

    game->quit = 0;
    game->frame = 0;

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
    update_terrain(terrain);

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
    new_orbit(&res->orbit_camera);

    node_set_label(root, "root");

    // ENTITIES

    // player entity
    init_id(&res->player_id);
    player = new_entity(&res->player_id);
    struct node *pnode = get_node(&player->node_id);
    assert(pnode);
    assert(res->player_id.index == 1);
    /* player->model_id = get_static_model(model_pirate_officer, NULL); */

    struct model *pmodel  = new_model(&player->model_id); assert(pmodel);
    struct geometry *geom = get_geometry(&pmodel->geom_id); assert(geom);

    proc_sphere(geom);
    pmodel->shading = SHADING_VERT_COLOR;

    node_set_label(pnode, "player");
    node_attach(&player->node_id, &res->root_id);
    assert(ideq(&pnode->parent_id, &res->root_id));

    node_translate(pnode, V3(terrain->size/2.,terrain->size/2.,0.0));

    // orbit camera
    res->orbit_camera.coords.azimuth = -quat_yaw(pnode->orientation) - RAD(90.0);
    res->orbit_camera.coords.inclination = RAD(60);
    res->orbit_camera.coords.radius = 5.0;
    res->camera_node_id = res->orbit_camera.node_id;

    // free camera
    struct node *freecam = new_node(&res->free_camera_id);
    node_set_label(freecam, "freecam");
    node_attach(&res->free_camera_id, &player->node_id);
    quat_axis_angle(V3(1,0,0), -45, freecam->orientation);
    node_rotate(freecam, V3(100, 0, 0));
    node_translate(freecam, V3(0,-40,20));

    input_init(&game->input);

    // FBO STUFF
    init_fbo(&res->shadow_buffer);
    resize_fbos(player, &res->shadow_buffer, res->proj_ortho, width, height);
    // FBO STUFF END

    // TEXTURES
    // END TEXTURES
}
