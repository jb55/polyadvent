
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
#include "poisson.h"
#include "procmesh.h"
#include "util.h"

#include <assert.h>

mat4 *cam_init = (float[16]){
  0.955761, 0.228018, -0.185425, 0.000000,
  -0.293528, 0.779583, -0.552437, 0.000000,
  0.018780, 0.580299, 0.802257, 0.000000,
  -71.766136, -47.881512, -44.216671, 1.000000
};

bool was_key_pressed_this_frame(struct game *game, int scancode)
{
    return is_key_down_on_frame(&game->input, scancode, game->frame);
}

bool was_button_pressed_this_frame(struct game *game, SDL_GameControllerButton button)
{
    return is_button_down_on_frame(&game->input, button, game->frame);
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
    SDL_Init( SDL_INIT_JOYSTICK );

    /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); */

    *window = SDL_CreateWindow("SDL2/OpenGL Demo", 0, 0, width, height,
                                          SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

    SDL_GL_CreateContext(*window);
}

void quit_game(struct game *game)
{
    game->quit = 1;
}

// TODO: cleanup
void init_misc(struct game *game, int width, int height) {
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

    game->quit = 0;
    game->frame = 0;

    const double size = 5000.0;
    double scale = 0.03;

    terrain->settings = (struct perlin_settings){
        .depth = 1,
        .freq  = scale * 0.08,
        .o1 = 2.0, .o1s = 0.5,
        .o2 = 4.0, .o2s = 0.25,
        .amplitude  = 70.0,
        .ox = 0,
        .oy = 0,
        .exp = 5.3,
        .scale = scale
    };

    create_ui(&game->ui, width, height, &res->programs[UI_PROGRAM]);

    check_gl();

    init_terrain(terrain, size);
    int seed;
    /* terrain->samples = load_samples(&seed, &terrain->n_samples); */
    terrain->n_samples = 0;
    create_terrain(terrain, size, game->seed);
    /* update_terrain(terrain, terrain->cell_size); */

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

    game->test_resources.fog_on = 0;
    game->test_resources.diffuse_on = 0;

    node_init(root);
    node_init(sun_camera);
    new_orbit(&res->orbit_camera);

    node_set_label(root, "root");

    // ENTITIES

    // player entity
    init_id(&res->player_id);
    player = &static_entities()[entity_player];
    player->flags |= ENT_CASTS_SHADOWS;
    struct node *pnode = &static_nodes()[node_player];
    assert(pnode);
    res->player_id = make_static_id(entity_player);
    assert(res->player_id.index == entity_player);

    struct model *pmodel;
    player->model_id = get_static_model(model_pirate_officer, &pmodel);

    node_set_label(pnode, "player");
    /* node_rotate(pnode, V3(-5.0,0,0)); */
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

    // FBO STUFF
    init_fbo(&res->shadow_buffer);
    resize_fbos(player, &res->shadow_buffer, res->proj_ortho, width, height);
    // FBO STUFF END

    // TEXTURES
    // END TEXTURES
}

void init_controller(struct input *input) {
    SDL_GameControllerAddMappingsFromFile("data/gamecontrollerdb.txt");

    int joysticks = SDL_NumJoysticks();
    SDL_GameController *controller = NULL;

    printf("Found %d joysticks\n", joysticks);

    for (int i = 0; i < joysticks; i++) {
        if (SDL_IsGameController(i)) {
            controller = SDL_GameControllerOpen(i);
            if (controller) {
                printf("Found a game controller\n");
                input->controller = controller;
                break;
            }
        } else {
            printf("Could not open game controller %d: %s\n", i, SDL_GetError());
        }
    }
}

void init_resources(struct resources *res)
{
    memset(res, 0, sizeof(*res));
}

void game_init(struct game *game, int width, int height) {
    init_resources(&game->test_resources);
    init_sdl(&game->window, width, height);
    init_gl(&game->test_resources, width, height);
    init_entity_system();
    init_geometry_manager();
    init_model_manager();
    init_node_manager();
    init_user_settings(&game->user_settings);

    check_gl();

    game->wireframe = 0;

    init_input(&game->input);
    init_controller(&game->input);
    init_misc(game, width, height);
}
