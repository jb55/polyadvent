
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


static void entity_movement(struct game *game, struct entity *ent)
{
    static const float move_accel = 1.0f;
    static const float max_speed = 10.0f;
    struct node *node = get_node(&ent->node_id);

    float vel[3];

    float amt = 10.0 * game->dt;

    if (game->input.keystates[SDL_SCANCODE_W]) {
        vec3_forward(ent->velocity, node->orientation, V3(0,amt,0), vel);
        if (vec3_lengthsq(vel) <= max_speed*max_speed)
            vec3_copy(vel, ent->velocity);
    }
}

static void movement(struct game *game, struct node *node, float speed_mult)
{
    float amt = 3.0 * game->dt;
    float turn = 1.0 * game->dt;

    float x_axis = (float)game->input.axis[0] / (float)MAX_AXIS_VALUE;
    float y_axis = (float)game->input.axis[1] / (float)MAX_AXIS_VALUE;

    amt *= speed_mult;

    if ((game->input.modifiers & KMOD_SHIFT) ||
        is_button_down(&game->input, SDL_CONTROLLER_BUTTON_LEFTSTICK))
    {
        amt *= 20;
    }

    // joystick movement
    node_forward(node, V3(0,amt*y_axis, 0));
    node_forward(node, V3(amt*x_axis, 0, 0));


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
        debug("player %f %f %f quat %f %f %f %f\n",
                node->pos[0],
                node->pos[1],
                node->pos[2],
                node->orientation[0],
                node->orientation[1],
                node->orientation[2],
                node->orientation[3]
              );
        struct spherical *cam = &game->test_resources.orbit_camera.coords;
        debug("camera settings radius %f inc %f azimuth %f\n",
              cam->radius, cam->inclination, cam->azimuth);
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

static void player_movement(struct game *game, struct entity *player) {
    /* if (player->flags & ENT_ON_GROUND) */
    /*     entity_movement(game, player); */
    struct node *node = get_node(&player->node_id);
    movement(game, node, 2.0);
}


#ifdef DEBUG
static int try_reload_shaders(struct resources *res) {
	int ret;
    for (int i = 0; i < NUM_PROGRAMS; ++i) {
        struct gpu_program *program = &res->programs[i];
        ret = reload_program(program, res->programs);

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
    const float factor = 40.5;

    struct model *model   = get_model(&player->model_id); assert(model);
    struct geometry *geom = get_geometry(&model->geom_id); assert(geom);

    float left   = geom->min[0] - factor;
    float right  = geom->max[0] + factor;
    float bottom = geom->min[1] - factor;
    float top    = geom->max[1] + factor;

    /* float left   = -factor; */
    /* float right  = factor; */
    /* float bottom = factor; */
    /* float top    = -factor; */

    const float near = -50.0;
    const float far = 50.0;

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

    if (player->flags & ENT_ON_GROUND)
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
    /* assert(player_geom->max[2] != 0); */
    vec3_add(target, V3(0.0, 0.0, -15.0), target);
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
                                     struct node *camera) {
    if (static_entities()[entity_terrain].flags & ENT_INVISIBLE)
        return;
    float move[3];
    float *camworld = node_world(camera);
    float pen;
    static const float penlim = 1.0;
    struct tri *tri = collide_terrain(terrain, camworld, move, &pen);

    if (!tri)
        return;

    if (pen < penlim) {
        float dir[3], above[3];
        vec3_normalize(move, dir);
        vec3_scale(dir, pen < 0 ? penlim : -penlim, above);
        vec3_add(camworld, move, camworld);
        vec3_add(camworld, above, camworld);
        /* vec3_add(move, above, move); */
        /* vec3_add(camworld, move, camworld); */
    }
}

static void entity_jump(struct entity *ent, float amount)
{
    float dir[3];
    debug("jumping\n");
    vec3_normalize(ent->velocity, dir);
    vec3_add(dir, V3(0, 0, amount), dir);
    vec3_add(ent->velocity, dir, ent->velocity);
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

    /* camera_keep_above_ground(&game->terrain, cam_node); */

    // move player camera toward camera orientation
    if (input_is_dragging(&game->input, SDL_BUTTON_RIGHT)) {
        float yaw = game->test_resources.orbit_camera.coords.azimuth;
        quat_axis_angle(V3(0.0, 0.0, 1.0), -yaw - RAD(90), node->orientation);
    }

    struct terrain *terrain = &game->terrain;

    /* player_terrain_collision(terrain, node); */

    float move[3];
    float pos[3];
    float pen;
    vec3_copy(node_world(node), pos);
    /* debug("node_world(player) %f %f %f\n", pos[0], pos[1], pos[2]); */
    /* struct tri *tri = collide_terrain(terrain, pos, move, &pen); */
    struct tri *tri = NULL;
    /* node_translate(node, move); */

    if (tri) {
        if (vec3_eq(move, V3(0,0,0), 0.1)) {
            player->flags |= ENT_ON_GROUND;
        }
        else if (pen < 0) {
            node_translate(node, move);
            /* vec3_all(player->velocity, 0); */
            vec3_scale(player->velocity, 0.1, player->velocity);
        }
        else {
            player->flags &= ~ENT_ON_GROUND;
        }
    }
    else {
        static int tric = 0;
        /* debug("%d no tri\n", tric++); */
    }

    /* if (player->flags & ENT_ON_GROUND && */
    /*     (was_key_pressed_this_frame(game, SDL_SCANCODE_SPACE) || */
    /*      was_button_pressed_this_frame(game, SDL_CONTROLLER_BUTTON_X))) { */
    /*     entity_jump(player, 2.0); */
    /* } */

    /* debug("player velocity %f %f %f\n", */
    /*       player->velocity[0], */
    /*       player->velocity[1], */
    /*       player->velocity[2]); */

    /* if (player->flags & ENT_AT_REST) */
    /*     vec3_scale(player->velocity, 0.00001, player->velocity); */

    /* node_translate(node, player->velocity); */
    node_recalc(node);
}



void update (struct game *game) {
	static int toggle_fog = 0;
	static int needs_terrain_update = 0;
	struct resources *res          = &game->test_resources;
    /* struct terrain   *terrain      = &game->terrain; */
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
		player_movement(game, player);
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
