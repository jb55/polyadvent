
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


static void qh_mesh_to_geom(qh_mesh_t *qh, struct make_geometry *geom) {
    assert(!geom->vertices);
    assert(!geom->indices);
    float *new_normals = malloc(sizeof(float) * 3 * qh->nvertices);

    geom->vertices = (float*)qh->vertices;
    geom->normals = (float*)qh->normals;
    geom->indices = qh->indices;
    geom->num_verts = qh->nvertices;
    geom->num_indices = qh->nindices;

    for (u32 i = 0; i < qh->nnormals; i++) {
        int ndv = i * 9;

        qh_vertex_t *n = &qh->normals[i];
        for (int j = 0; j < 9; j++) {
            new_normals[ndv+j] = n->v[j%3];
        }
    }

    geom->normals = new_normals;
}


void proc_sphere(struct make_geometry *mkgeom, geometry_id *geom_id) {
    const int n = 50;
    qh_vertex_t *vertices = malloc(n*sizeof(qh_vertex_t));
    const float radius = 2.0;


    for (int i = 0; i < n; ++i) {
        float a0 = (rand_0to1() * TAU);
        float a1 = (rand_0to1() * TAU);
        vertices[i].z = sin(a0) * radius;
        vertices[i].x = cos(a1) * cos(a0) * rand_0to1() * radius;
        vertices[i].y = sin(a1) * cos(a0) * rand_0to1() * radius;
    }

    qh_mesh_t mesh = qh_quickhull3d(vertices, n);
    qh_mesh_to_geom(&mesh, mkgeom);
    make_buffer_geometry(mkgeom, geom_id);

    qh_free_mesh(mesh);
}


void game_init(struct game *game, int width, int height) {
    init_gl(&game->test_resources, width, height);
    init_entity_system();
    init_model_manager();
    init_geometry_manager();
    init_user_settings(&game->user_settings);
    check_gl();

    struct resources *res = &game->test_resources;
    mat4 *mvp = res->test_mvp;
    struct node *root = &res->root;
    struct node *sun_camera = &res->sun_camera;
    struct terrain *terrain = &game->terrain;
    struct entity *player;

    mat4 *light_dir = res->light_dir;
    int ok = 0;

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
    init_orbit(&res->orbit_camera);

    root->label = "root";


    // ENTITIES

    // player entity
    player = new_entity(&res->player_id);
    assert(res->player_id.index == 1);
    player->model = get_model(model_pirate_officer);
    player->node.label = "player";
    node_attach(&player->node, root);
    node_translate(&player->node, V3(terrain->size/2.,terrain->size/2.,0.0));

    // orbit camera
    res->orbit_camera.coords.azimuth = -quat_yaw(player->node.orientation) - RAD(90.0);
    res->orbit_camera.coords.inclination = RAD(60);
    res->orbit_camera.coords.radius = 5.0;
    res->camera_node = &res->orbit_camera.node;

    // free camera
    node_init(&res->free_camera);
    res->free_camera.label = "free_camera";
    node_attach(&res->free_camera, &player->node);
    quat_axis_angle(V3(1,0,0), -45, res->free_camera.orientation);
    node_rotate(&res->free_camera, V3(100, 0, 0));
    node_translate(&res->free_camera, V3(0,-40,20));

    input_init(&game->input);

    // FBO STUFF
    init_fbo(&res->shadow_buffer);
    resize_fbos(player, &res->shadow_buffer, res->proj_ortho, width, height);
    // FBO STUFF END

    // TEXTURES
    // END TEXTURES
}
