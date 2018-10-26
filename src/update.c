
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

  // TODO: mark as update
  /* if (game->input.keystates[SDL_SCANCODE_UP]) */
  /*   node_rotate(node, V3(amt * 0.01,0,0)); */

  if (game->input.keystates[SDL_SCANCODE_E])
    node_rotate(node, V3(0, 0, turn));

  if (game->input.keystates[SDL_SCANCODE_Q])
    node_rotate(node, V3(0, 0, -turn));

  /* if (game->input.keystates[SDL_SCANCODE_DOWN]) */
  /*   node_rotate(node, V3(-amt * 0.01, 0, 0)); */

  if (game->input.keystates[SDL_SCANCODE_P]) {
    printf("%f %f %f\n",
           node->pos[0],
           node->pos[1],
           node->pos[2]);
    mat4_print(node->mat);
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

    struct point *samples =
      poisson_disk_samples(42.0, game->terrain.size, 30, &n_samples);

    terrain->samples = samples;
    terrain->n_samples = n_samples;
  }

  last_scale = scale;
  terrain_create(&game->terrain);
}


static void player_movement(struct game *game) {
  struct resources *res = &game->test_resources;

  // player movement
  static vec3 last_pos[3] = {0};

  movement(game, &res->player.node, 1.0);

  if (!vec3_eq(res->player.node.pos, last_pos, 0.0001)) {

    res->player.node.pos[2] =
      game->terrain.fn(&game->terrain, res->player.node.pos[0], res->player.node.pos[1]) + 0.2;

    node_recalc(&res->camera);

    vec3_copy(res->player.node.pos, last_pos);
  }

  node_recalc(&res->camera);
  vec3 *camera_world = node_world(&res->camera);
  float cam_terrain_z =
    game->terrain.fn(&game->terrain, camera_world[0], camera_world[1]);

  if (camera_world[2] < cam_terrain_z)
    camera_world[2] = cam_terrain_z + 20.0;
}


#ifdef DEBUG
static int try_reload_shaders(struct resources *res) {
	int ret;
	printf("reloading shaders... ");

	ret = reload_program(&res->program);

	if (ret == 2)
		printf("nothing to reload\n");
	else if (ret == 1)
		printf("success.\n");
	else
		printf("failed.\n");



	return ret;
}
#endif

static void day_night_cycle(float n, struct resources *res) {
    float darkest = 0.25;
    float val = n;
    float roots = sin(val);
    float circle = fmod(val, TAU);
    float angle = circle/TAU;
    float hour = 24.0*angle;
    float intensity = angle <= 0.5
        ? clamp(roots, darkest, 1.0)
        : clamp(-roots * 0.4, darkest, 0.5);
    printf("intensity %f(%f) angle %f hour %f n %f\n", roots, intensity, angle,
           hour, n);

    res->light_intensity[0] = intensity;
    res->light_intensity[1] = intensity;
    res->light_intensity[2] = intensity;

    res->light_dir[0] = -cos(val);
    res->light_dir[1] = -sin(val);
    res->light_dir[2] = 0.8;
}

void update (struct game *game, u32 dt) {
	static double last_ox, last_oy, last_oz;
	static int toggle_fog = 0;
	static float n = 1.0;
	static int first = 1;
	struct resources *res = &game->test_resources;
	struct perlin_settings *ts = &game->terrain.settings;
	struct node *tnode = &game->terrain.entity.node;
	struct node *root = &game->test_resources.root;
	float *light = res->light_dir;

	if (first) {
		update_terrain(game);
		first = 0;
	}

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

	double ox = tnode->pos[0];
	double oy = tnode->pos[1];

	bool changed = last_ox != ox || last_oy != oy || last_oz != tnode->pos[2];

	if (changed) {
		update_terrain(game);
		last_ox = ts->ox = ox;
		last_oy = ts->oy = oy;
		last_oz = tnode->pos[2] = max(tnode->pos[2], 5.0);

		n += 0.01f;
	}

    day_night_cycle(n, res);

	n += 0.00001f;

	node_recalc(root);
}
