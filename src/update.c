
#include "gl.h"
#include "update.h"
#include "terrain.h"
#include "util.h"
#include "mat4.h"
#include "vec3.h"
#include "camera.h"
#include "poisson.h"
#include "uniform.h"
#include "shader.h"
#include "file.h"
#include <math.h>

static void movement(struct game *game, struct node *node, float speed_mult) {
  float amt = 0.03;
  static const float turn = 0.01;

  amt *= speed_mult;

  if (game->input.modifiers & KMOD_SHIFT)
    amt *= 50;

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

  if (game->input.keystates[SDL_SCANCODE_R])
    node_translate(node, V3(0, 0,amt));

  if (game->input.keystates[SDL_SCANCODE_F])
    node_translate(node, V3(0, 0,-amt));

  if (game->input.keystates[SDL_SCANCODE_E])
    node_rotate(node, V3(0, 0, turn));

  if (game->input.keystates[SDL_SCANCODE_Q])
    node_rotate(node, V3(0, 0, -turn));

  /* if (game->input.keystates[SDL_SCANCODE_DOWN]) */
  /*   node_translate(node, V3(0, 0, -amt)); */

  if (game->input.keystates[SDL_SCANCODE_P]) {
    printf("%f %f %f\n",
           node->pos[0],
           node->pos[1],
           node->pos[2]);
    mat4_print(node->mat);
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

void update_terrain(struct game *game) {
  static int first = 1;
  static float last_scale = -1.0;

  struct perlin_settings *ts = &game->terrain.settings;
  struct node *tnode = &game->terrain.entity.node;
  struct terrain *terrain = &game->terrain;

  printf("updating terrain\n");

  if (first) {
    terrain_init(terrain);
    tnode->pos[0] = rand_0to1() * 100000.0;
    tnode->pos[1] = rand_0to1() * 100000.0;
    first = 0;
  }

  ts->ox = tnode->pos[0];
  ts->oy = tnode->pos[1];

  double scale = tnode->pos[2] * 0.0015;
  if (scale == 0) scale = 1.0;

  printf("terrain %f %f %f\n", tnode->pos[0], tnode->pos[1], tnode->pos[2]);

  /* ts.o1s = fabs(sin(1/n) * 0.25); */
  /* ts.o1 = fabs(cos(n*0.2) * 0.5); */
  /* ts.o2s = fabs(cos(n+2) * 0.5); */
  /* ts.o2 = fabs(sin(n*0.02) * 2); */
  ts->freq = scale * 0.05;
  ts->amplitude = 50.0;

  if (terrain->fn)
    terrain_destroy(&game->terrain);


  /* const double pdist = min(5.0, max(1.1, 1.0/scale*1.4)); */

  /* printf("pdist %f\n", pdist); */

  if (last_scale == -1.0 || fabs(scale - last_scale) > 0.00001) {
    printf("generating new samples\n");

    if (terrain->samples)
      free(terrain->samples);

    int n_samples =
      (terrain->size * game->terrain.size) / (scale * scale);

    /* struct point *samples = */
    /*   uniform_samples(n_samples, game->terrain.size); */

    static const double pdist = 24.0;

    struct point *samples =
      poisson_disk_samples(pdist, game->terrain.size, 30, &n_samples);

    /* remap_samples(samples, n_samples, game->terrain.size); */

    /* draw_samples(samples, pdist, n_samples, game->terrain.size); */

    terrain->samples = samples;
    terrain->n_samples = n_samples;
  }

  last_scale = scale;
  terrain_create(&game->terrain);
}

static void player_terrain_collision(struct terrain *terrain, struct entity *player) {
    // player movement
    static vec3 last_pos[3] = {0};

    if (!vec3_eq(player->node.pos, last_pos, 0.0001)) {
        float player_z = player->node.pos[2];

        float terrain_z = 
            terrain->fn(terrain, player->node.pos[0], player->node.pos[1]) + 5.0;

        float inset =
            min(0.0, player_z - terrain_z);

        if (inset <= 0)
            node_translate(&player->node, V3(0.0, 0.0, -inset));
    }

}

static void player_movement(struct game *game) {
  struct resources *res = &game->test_resources;
  movement(game, &res->player.node, 2.0);

  /* vec3 *camera_world = node_world(&res->camera); */
  /* float cam_terrain_z = */
  /*   game->terrain.fn(&game->terrain, camera_world[0], camera_world[1]); */

  /* if (camera_world[2] < cam_terrain_z) */
  /*   camera_world[2] = cam_terrain_z + 20.0; */
}


#ifdef DEBUG
static int try_reload_shaders(struct resources *res) {
	int ret;
    for (int i = 0; i < NUM_PROGRAMS; ++i) {
        struct gpu_program *program = &res->programs[i];
        ret = reload_program(program);

        if (ret == 2) {}
        else if (ret == 1)
            printf("reload %s + %s success.\n",
                   program->vertex.filename,
                   program->fragment.filename);
        else
            printf("reload %s + %s failed.\n",
                   program->vertex.filename,
                   program->fragment.filename);

        // failure ok, clear any errors
        glGetError();
    }




	return ret;
}
#endif

void resize_fbos(struct game *game, int width, int height) {
    struct resources *res = &game->test_resources;

    if (res->shadow_buffer.handle) {
        // TODO: remove once delete_fbo deletes attachments
        glDeleteTextures(1, &res->shadow_buffer.attachments[1]);
        glDeleteRenderbuffers(1, &res->shadow_buffer.attachments[0]);
        delete_fbo(&res->shadow_buffer);
    }

    create_fbo(&res->shadow_buffer, width, height);
    /* fbo_attach_renderbuffer(&res->shadow_buffer, GL_DEPTH24_STENCIL8, */
    /*                         GL_DEPTH_STENCIL_ATTACHMENT); */

    /* fbo_attach_color_texture(&res->shadow_buffer); */
    fbo_attach_depth_texture(&res->shadow_buffer);

    check_fbo(&res->shadow_buffer);

    /* fbo_attach_texture(&res->shadow_buffer, GL_DEPTH_COMPONENT16, */
    /*                    GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT); */

}

// TODO: match based on some real concept of time
static void day_night_cycle(float time, struct resources *res) {
    float val = time;
    float intensity = vec3_dot(res->light_dir, V3(0.0, 0.0, 1.0));
    intensity = clamp(intensity, 0.0, 0.6);
    float light_pos[3];

    res->sun_color[0] = 1.0;
    res->sun_color[1] = 0.9;
    res->sun_color[2] = 0.8;

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
    res->light_dir[1] = sin(val);
    res->light_dir[2] = cos(val) + 1.0;

    vec3_normalize(res->light_dir, res->light_dir);

    /* printf("intensity %f(%f) n %f light_dir %f %f\n", roots, intensity, */
    /*        n, res->light_dir[1], res->light_dir[2]); */

    vec3_add(res->player.node.pos, res->light_dir, light_pos);

    look_at(light_pos, res->player.node.pos, V3(0, 0, 1.0), res->sun_camera.mat);
}

static void gravity(struct game *game) {
    struct entity *player = &game->test_resources.player;

    node_translate(&player->node, V3(0.0, 0.0, -1.0));
}

static void player_update(struct game *game, struct entity *player) {
    struct orbit *orbit_camera = &game->test_resources.orbit_camera;
    /* orbit_camera->radius += game->dt * 4.0; */
    /* orbit_camera->inclination += game->dt * 2.0; */
    /* orbit_camera->azimuth += game->dt * 2.0; */
    /* orbit_to_node(orbit_camera, &game->test_resources.camera); */

    player_terrain_collision(&game->terrain, player);
}

void update (struct game *game) {
	static int toggle_fog = 0;
	static int first = 1;
	struct resources *res = &game->test_resources;
	struct node *tnode = &game->terrain.entity.node;
	struct node *root = &game->test_resources.root;
    float *time = &res->time;
	float *light = res->light_dir;

    gravity(game);

	if (first) {
		update_terrain(game);
		first = 0;
	}

    player_update(game, &res->player);

	if (game->input.modifiers & KMOD_LALT) {
		movement(game, &res->camera, 1.0);
	}
	else if (game->input.modifiers & KMOD_RCTRL) {
		movement(game, tnode, 5.0);
	}
	else {
		player_movement(game);
	}


#ifdef DEBUG
	if (game->input.keystates[SDL_SCANCODE_R])
		try_reload_shaders(res);
#endif

	if (game->input.keystates[SDL_SCANCODE_C])
		printf("light_dir %f %f %f\n", light[0], light[1], light[2]);

	if (game->input.keystates[SDL_SCANCODE_F])
		toggle_fog = 1;

	if (toggle_fog) {
		res->fog_on = !res->fog_on;
		toggle_fog = 0;
	}

    day_night_cycle(*time, res);

	*time += game->dt * 0.00001f;

	node_recalc(root);

}
