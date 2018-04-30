
#include "gl.h"
#include "update.h"
#include "terrain.h"
#include "util.h"
#include "mat4.h"
#include "vec3.h"
#include "camera.h"
#include "poisson.h"
#include "uniform.h"

void movement(struct game *game, struct node *node) {
  float amt = 0.25;

  if (game->input.modifiers & KMOD_SHIFT)
    amt *= 6;

  if (game->input.keystates[SDL_SCANCODE_A])
    node_translate(node, V3(-amt,0,0));

  if (game->input.keystates[SDL_SCANCODE_E])
    node_translate(node, V3(0,0,amt));

  if (game->input.keystates[SDL_SCANCODE_Q])
    node_translate(node, V3(0,0,-amt));

  if (game->input.keystates[SDL_SCANCODE_D])
    node_translate(node, V3(amt,0,0));

  if (game->input.keystates[SDL_SCANCODE_W])
    node_translate(node, V3(0,amt,0));

  if (game->input.keystates[SDL_SCANCODE_S])
    node_translate(node, V3(0,-amt,0));

  if (game->input.keystates[SDL_SCANCODE_UP])
    node->rot[0] += amt * 0.01;

  if (game->input.keystates[SDL_SCANCODE_RIGHT])
    node->rot[2] -= amt * 0.01;

  if (game->input.keystates[SDL_SCANCODE_LEFT])
    node->rot[2] += amt * 0.01;

  if (game->input.keystates[SDL_SCANCODE_DOWN])
    node->rot[0] -= amt * 0.01;

  /* if (game->input.keystates[SDL_SCANCODE_P]) */
  /*   mat4_print(obj); */
}

void update (struct game *game, u32 dt) {
  static int passed = 0;
  static int last_gen_time = 50;
  static float n = 1;
  static float nn = 1;
  static int first = 1;
  static double last_ox, last_oy, last_oz;
  struct resources *res = &game->test_resources;
  static int stopped = 0;
  struct perlin_settings *ts = &game->terrain->settings;
  float player_prev[MAT4_ELEMS];
  struct node *tnode = &game->test_resources.terrain_node;
  float *light = res->light_dir;

  if (first) {
    ts->ox = rand_0to1() * 1000.0;
    ts->oy = rand_0to1() * 1000.0;
    first = 0;
  }

  if (game->input.modifiers & KMOD_LALT) {
    movement(game, &res->camera);
    /* mat4_multiply(res->player, res->ca era, res->player); */
  }
  if (game->input.modifiers & KMOD_LCTRL) {
    movement(game, &res->terrain_node);
    /* mat4_multiply(res->player, res->ca era, res->player); */
  }
  else {
    movement(game, &res->player);
    /* mat4_copy(res->player, player_prev); */

    res->player.pos[2] =
      game->terrain->fn(game->terrain, res->player.pos[0], res->player.pos[1]) +
        PLAYER_HEIGHT;
    res->player.needs_recalc = 1;

    node_recalc(&res->camera);
    camera_follow(res->camera.pos, res->player.pos, res->player.pos, res->camera.mat);
    res->camera.needs_recalc = 1;
    /* movement(game, res->camera); */
  }

  if (game->input.keystates[SDL_SCANCODE_C]) {
    printf("light_dir %f %f %f\n", light[0], light[1], light[2]);

  }

  int space_down = game->input.keystates[SDL_SCANCODE_SPACE];
  int ctrl_down = game->input.modifiers & KMOD_CTRL;
  int now = SDL_GetTicks();

  if (space_down) {
    if (!stopped) {
      printf("terrain amp %f exp %f freq %f (%d ms)\n",
             ts->amplitude,
             ts->exp,
             ts->freq,
             last_gen_time);
      stopped = 1;
    }
    else {
      stopped = 0;
    }
  }

  if (space_down || passed < last_gen_time) {
    passed += dt;
  } else {
    passed = 0;

    double ox = tnode->pos[0];
    double oy = tnode->pos[1];

    bool changed = last_ox != ox || last_oy != oy || last_oz != tnode->pos[2];

    if (!stopped && changed) {
      for (int i = 0; i < 2; ++i)
        tnode->pos[i] = max(tnode->pos[i], 0);

      last_oz = tnode->pos[2] = max(tnode->pos[2], 20.0);

      double scale = tnode->pos[2] * 0.01;
      if (scale == 0) scale = 1.0;

      printf("terrain %f %f %f\n", tnode->pos[0], tnode->pos[1], tnode->pos[2]);

      last_ox = ts->ox = ox;
      last_oy = ts->oy = oy;

      ts->scale = scale;
      /* ts.o1s = fabs(sin(1/n) * 0.25); */
      /* ts.o1 = fabs(cos(n*0.2) * 0.5); */
      /* ts.o2s = fabs(cos(n+2) * 0.5); */
      /* ts.o2 = fabs(sin(n*0.02) * 2); */
      ts->freq = scale * 0.15;

      ts->amplitude = 1/scale;

      terrain_destroy(game->terrain);
      terrain_init(game->terrain);

      int t1 = SDL_GetTicks();
      free(game->terrain->samples);

      /* const double pdist = min(5.0, max(1.1, 1.0/scale*1.4)); */

      /* printf("pdist %f\n", pdist); */

      int n_samples =
        (game->terrain->size * game->terrain->size) * scale*scale;

      struct point *samples =
        uniform_samples(n_samples, game->terrain->size);

      game->terrain->samples = samples;
      game->terrain->n_samples = n_samples;

      terrain_create(game->terrain);
      int t2 = SDL_GetTicks();
      last_gen_time = t2 - t1;

      n += 0.01f;
    }

  }

}
