
#include "gl.h"
#include "update.h"
#include "terrain_collision.h"
#include "util.h"
#include "mat4.h"
#include "vec3.h"
#include "camera.h"
#include "poisson.h"
#include "uniform.h"
#include "game.h"
#include "mat_util.h"
#include "resource.h"
#include "shader.h"
#include "file.h"
#include "debug.h"
#include <math.h>

static void movement(struct game *game, struct node *node, float speed_mult) {
    float amt = 3.0 * game->dt;
    float turn = 1.0 * game->dt;

    amt *= speed_mult;

    if (game->input.modifiers & KMOD_SHIFT)
        amt *= 20;

    if (game->input.keystates[SDL_SCANCODE_A])
        node_forward(node, V3(-amt,0,0));

    if (game->input.keystates[SDL_SCANCODE_UP])
        node_forward(node, V3(0,0,amt));

    if (game->input.keystates[SDL_SCANCODE_DOWN])
        node_forward(node, V3(0,0,-amt));

    if (game->input.keystates[SDL_SCANCODE_D])
        node_forward(node, V3(amt,0,0));

    if (game->input.keystates[SDL_SCANCODE_W])
        node_forward(node, V3(0,amt,0));

    if (game->input.keystates[SDL_SCANCODE_S])
        node_forward(node, V3(0,-amt,0));

    if (game->input.keystates[SDL_SCANCODE_K])
        node_forward(node, V3(0, 0,amt));

    if (game->input.keystates[SDL_SCANCODE_J])
        node_forward(node, V3(0, 0,-amt));

    if (game->input.keystates[SDL_SCANCODE_E])
        node_rotate(node, V3(0, 0, turn));

    if (game->input.keystates[SDL_SCANCODE_Q])
        node_rotate(node, V3(0, 0, -turn));

    /* if (game->input.keystates[SDL_SCANCODE_DOWN]) */
    /*   node_translate(node, V3(0, 0, -amt)); */

    if (was_key_pressed_this_frame(game, SDL_SCANCODE_P)) {
        debug("%f %f %f ",
                node->pos[0],
                node->pos[1],
                node->pos[2]);
        node_recalc(node);
        grid_pos_debug(node->label, &game->terrain, node_world(node));
        /* mat4_print(node->mat); */
    }
}

static void remap_samples(struct point *points, int n_samples,
                                    double size)
{
    double middle = size / 2.0;

    for (int i = 0; i < n_samples; ++i) {
        struct point *point = &points[i];

        /* double x = point->x/size; */
        /* double y = point->y/size; */
        double dx = point->x - middle;
        double dy = point->y - middle;
        double dist = sqrt(dx*dx + dy*dy);
        double nx = dx / dist;
        double ny = dy / dist;
        double factor = -log(dist)*50.0;
        point->x += nx * factor;
        point->y += ny * factor;
    }
}

static void player_terrain_collision(struct terrain *terrain, struct node *node) {
    // player movement
    static vec3 last_pos[3] = {0};

    if (!vec3_approxeq(node->pos, last_pos)) {
        float player_z = node->pos[2];

        float terrain_z =
            terrain->fn(terrain, node->pos[0], node->pos[1]);

        float inset =
            min(0.0, player_z - terrain_z);

        if (inset <= 0)
            node_translate(node, V3(0.0, 0.0, -inset));
    }

}

static void player_movement(struct game *game, struct node *player_node) {
    movement(game, player_node, 2.0);
}


#ifdef DEBUG
static int try_reload_shaders(struct resources *res) {
	int ret;
    for (int i = 0; i < NUM_PROGRAMS; ++i) {
        struct gpu_program *program = &res->programs[i];
        ret = reload_program(program);

        if (ret == 2) {}
        else if (ret == 1)
            printf("reload %s success.\n", program->shaders[0].filename);
        else
            printf("reload %s failed.\n", program->shaders[0].filename);

        // failure ok, clear any errors
        glGetError();
    }




	return ret;
}
#endif

void resize_fbos(struct entity *player, struct fbo *shadow_buffer,
                 float *m4_ortho, int width, int height)
{
    if (shadow_buffer->handle) {
        // TODO: remove once delete_fbo deletes attachments
        glDeleteTextures(1, &shadow_buffer->attachments[1]);
        glDeleteRenderbuffers(1, &shadow_buffer->attachments[0]);
        delete_fbo(shadow_buffer);
    }

    // TODO: compute better bounds based
    const float factor = 10.5;

    struct model *model   = get_model(&player->model_id); assert(model);
    struct geometry *geom = get_geometry(&model->geom_id); assert(geom);

    float left   = geom->min[0] - factor;
    float right  = geom->max[0] + factor;
    float bottom = geom->min[1] - factor;
    float top    = geom->max[1] + factor/2.0;

    /* float left   = -factor; */
    /* float right  = factor; */
    /* float bottom = factor; */
    /* float top    = -factor; */

    const float near = -5.0;
    const float far = 5.0;

    // default ortho screenspace projection
    mat4_ortho(left, right, bottom, top, near, far, m4_ortho);

    create_fbo(shadow_buffer, width, height );
    /* fbo_attach_renderbuffer(&res->shadow_buffer, GL_DEPTH24_STENCIL8, */
    /*                         GL_DEPTH_STENCIL_ATTACHMENT); */

    /* fbo_attach_color_texture(&res->shadow_buffer); */
    fbo_attach_depth_texture(shadow_buffer);

    check_fbo(shadow_buffer);

    /* fbo_attach_texture(&res->shadow_buffer, GL_DEPTH_COMPONENT16, */
    /*                    GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT); */

}

// TODO: match based on some real concept of time
static void day_night_cycle(float time, struct resources *res) {
    float val = time * 0.0001;
    float intensity = 1.0;//max(0.0, vec3_dot(res->light_dir, V3(0.0, 0.0, 1.0))); */
    struct entity *player = get_player(res);
    struct node *pnode = get_node(&player->node_id);
    assert(pnode);
    struct node *suncam = get_node(&res->sun_camera_id);
    assert(suncam);

    float light_pos[3];

    float g = 0.6;
    float b = 0.4;
    res->sun_color[0] = 1.0;
    res->sun_color[1] = g+intensity*(1.0-g);
    res->sun_color[2] = b+intensity*(1.0-b);

    /* res->sun_color[0] = 1.0; */
    /* res->sun_color[1] = 1.0; */
    /* res->sun_color[2] = 1.0; */

    /* vec3_scale(res->sun_color, res->light_intensity, gtmp); */

    /* float intensity = angle <= 0.5 */
    /*     ? clamp(roots, darkest, 1.0) */
    /*     : clamp(-roots * 0.4, darkest, 0.5); */

    res->light_intensity = intensity;

    /* vec3_normalize(res->light_intensity, res->light_intensity); */

    res->light_dir[0] = 0.0;
    /* res->light_dir[1] = sin(val); */
    /* res->light_dir[2] = cos(val) + 1.0; */
    res->light_dir[1] = 0.8;
    res->light_dir[2] = 0.8;

    vec3_normalize(res->light_dir, res->light_dir);

    /* printf("intensity %f(%f) n %f light_dir %f %f\n", roots, intensity, */
    /*        n, res->light_dir[1], res->light_dir[2]); */

    vec3_add(pnode->pos, res->light_dir, light_pos);

    /* float target[3]; */
    /* float hh = player->model.geom.max[2] / 2.0; */
    /* vec3_copy(player->node.pos, target); */
    /* target[2] += 2.0; */

    look_at(light_pos, pnode->pos, V3(0, 0, 1.0), suncam->mat);
    /* look_at(light_pos, player->node.pos, V3(0, 0, 1.0), res->sun_camera.mat); */
}

static void gravity(struct game *game) {
    struct entity *player = get_player(&game->test_resources);

    if (player->flags & ENT_AT_REST)
        return;

    struct node   *pnode  = get_node(&player->node_id);
    assert(pnode);

    static const float g = -1.0;
    vec3_add(player->velocity, V3(0.0, 0.0, g * game->dt), player->velocity);
}

void orbit_update_from_mouse(struct orbit *camera, struct input *input,
                             float mouse_sens, struct entity *player,
                             float dt)
{
    float target[3];
    struct node *target_node     = get_node(&player->node_id);
    struct node *cam_node        = get_node(&camera->node_id);
    struct model *pmodel         = get_model(&player->model_id); assert(pmodel);
    struct geometry *player_geom = get_geometry(&pmodel->geom_id); assert(player_geom);

    assert(target_node);
    assert(cam_node);

    node_recalc(target_node);
    vec3_copy(node_world(target_node), target);
    assert(player_geom->max[2] != 0);
    vec3_add(target, V3(0.0, 0.0, player_geom->max[2]), target);
    /* vec3_add(target, V3(0.0, 0.0, 10.0), target); */

    float mx = 0.0, my = 0.0;
    if (input_is_dragging(input, SDL_BUTTON_LEFT) ||
        input_is_dragging(input, SDL_BUTTON_RIGHT)) {
        mx = -input->mdx * mouse_sens * dt;
        my = -input->mdy * mouse_sens * dt;
    }

    // zoom
    if (input->keystates[SDL_SCANCODE_V]) {
        if (input->modifiers & KMOD_SHIFT)
            camera->coords.radius += dt * 100.0;
        else
            camera->coords.radius -= dt * 100.0;

    }
    else if (input->wheel_y) {
        camera->coords.radius += input->wheel_y * dt * 100.0;
    }

    camera->coords.radius = max(1.0, camera->coords.radius);

    camera->coords.azimuth     += mx;
    camera->coords.inclination += my;
    /* printf("coords azimuth %f inclination %f radius %f\n", */
    /*        camera->coords.azimuth, */
    /*        camera->coords.inclination, */
    /*        camera->coords.radius); */

    spherical_look_at(&camera->coords, target, cam_node->mat);

}

static void camera_keep_above_ground(struct terrain *terrain,
                                     const struct node *camera) {
    if (!(get_entity(&terrain->entity_id)->flags & ENT_INVISIBLE)) {
        float *camera_world = node_world((struct node*)camera);
        /* float *target = node_world(&get_player(res)->node); */
        /* spherical_pos(&res->camera.coords, target, camera_world); */
        float cam_terrain_z =
            terrain->fn(terrain, camera_world[0], camera_world[1]);

        const float bias = 1.0;

        if (camera_world[2] < cam_terrain_z + bias)
            camera_world[2] = cam_terrain_z + bias;
    }
}

static void entity_jump(struct entity *ent)
{
    if (ent->flags & ENT_AT_REST)  {
        struct node *node = get_node(&ent->node_id);
        debug("jumping\n");
        vec3_forward(ent->velocity, node->orientation, V3(0.0, 0.0, 1.0), ent->velocity);
    }
}

static void player_update(struct game *game, struct entity *player)
{

    struct resources *res = &game->test_resources;
    struct orbit *camera = &res->orbit_camera;
    struct node *node = get_node(&player->node_id);
    struct node *cam_node = get_node(&res->camera_node_id);
    assert(node);
    assert(cam_node);

    orbit_update_from_mouse(camera, &game->input, game->user_settings.mouse_sens,
                            player, game->dt);

    camera_keep_above_ground(&game->terrain, cam_node);

    // move player camera toward camera orientation
    if (input_is_dragging(&game->input, SDL_BUTTON_RIGHT)) {
        float yaw = game->test_resources.orbit_camera.coords.azimuth;
        quat_axis_angle(V3(0.0, 0.0, 1.0), -yaw - RAD(90), node->orientation);
    }

    struct terrain *terrain = &game->terrain;

    /* player_terrain_collision(terrain, node); */

    float move[3];
    float scaled[3];
    struct tri *tri = collide_terrain(terrain, node_world(node), NULL, move);
    /* node_translate(node, move); */

    if (tri) {
        if (move[2] >= 0) {
            node_translate(node, move);
            player->flags |= ENT_AT_REST;
            vec3_all(player->velocity, 0);
        }
        else {
            player->flags &= ~ENT_AT_REST;
        }
    }

    if (was_key_pressed_this_frame(game, SDL_SCANCODE_SPACE)) {
        entity_jump(player);
    }

    if (!vec3_approxeq(player->velocity, V3(0,0,0)))
        player->flags &= ~ENT_AT_REST;

    /* debug("player velocity %f %f %f\n", */
    /*       player->velocity[0], */
    /*       player->velocity[1], */
    /*       player->velocity[2]); */

    /* if (player->flags & ENT_AT_REST) */
    /*     vec3_scale(player->velocity, 0.00001, player->velocity); */

    if (!(player->flags & ENT_AT_REST)) {
        debug("player not a rest\n");
        node_translate(node, player->velocity);
        node_recalc(node);
    }
}



void update (struct game *game) {
	static int toggle_fog = 0;
	static int needs_terrain_update = 0;
	struct resources *res          = &game->test_resources;
    struct terrain   *terrain      = &game->terrain;
	struct node      *root         = get_node(&game->test_resources.root_id);
    struct entity    *player       = get_player(res);
    struct node      *pnode        = get_node(&player->node_id);
    struct node      *cam_node     = get_node(&res->camera_node_id);

    assert(pnode);
    assert(cam_node);

    float *time = &res->time;
	float *light = res->light_dir;

    gravity(game);

	if (needs_terrain_update) {
		/* update_terrain(terrain); */
		needs_terrain_update = 0;
	}

    /* spherical_dir(game->test_resources.camera.coords, camera_dir); */
    /* vec3_scale(camera_dir, -1, camera_dir); */

    if (game->input.modifiers & KMOD_ALT &&
        ideq(&res->camera_node_id, &res->free_camera_id))
    {
        struct node *freecam_node = get_node(&res->free_camera_id);
        assert(freecam_node);
        assert(streq(freecam_node->label, "freecam"));
        movement(game, freecam_node, 1.0);
    }
	else {
		player_movement(game, pnode);
	}

    assert(root->parent_id.generation == 0);

    player_update(game, player);

#ifdef DEBUG
	if (was_key_pressed_this_frame(game, SDL_SCANCODE_R))
		try_reload_shaders(res);
#endif

    if (was_key_pressed_this_frame(game, SDL_SCANCODE_F5)) {
        game->wireframe ^= 1;
    }

	if (was_key_pressed_this_frame(game, SDL_SCANCODE_C)) {
		printf("light_dir %f %f %f\n", light[0], light[1], light[2]);
    }

	if (was_key_pressed_this_frame(game, SDL_SCANCODE_F))
		toggle_fog = 1;

	if (was_key_pressed_this_frame(game, SDL_SCANCODE_EQUALS)) {
        if (!ideq(&res->camera_node_id, &res->free_camera_id)) {
            debug("switching to freecam\n");
            res->camera_node_id = res->free_camera_id;
        }
        else {
            debug("switching to orbitcam\n");
            res->camera_node_id = res->orbit_camera.node_id;
        }
    }

	if (toggle_fog) {
		res->fog_on = !res->fog_on;
		toggle_fog = 0;
	}

    /* for (int i = 0; i < ) */

	*time = SDL_GetTicks();

    day_night_cycle(*time, res);

	node_recalc(root);
}
