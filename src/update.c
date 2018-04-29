
#include "gl.h"
#include "update.h"
#include "terrain.h"
#include "util.h"
#include "mat4/mat4.h"
#include "vec3/vec3.h"
#include "poisson.h"

void movement(struct game *game, float *obj) {
  float amt = 0.25;

  if (game->input.modifiers & KMOD_SHIFT)
    amt *= 6;

  if (game->input.keystates[SDL_SCANCODE_A])
    mat4_translate(obj, -amt, 0, 0, obj);

  if (game->input.keystates[SDL_SCANCODE_E])
    mat4_translate(obj, 0, 0, amt, obj);

  if (game->input.keystates[SDL_SCANCODE_Q])
    mat4_translate(obj, 0, 0, -amt, obj);

  if (game->input.keystates[SDL_SCANCODE_D])
    mat4_translate(obj, amt, 0, 0, obj);

  if (game->input.keystates[SDL_SCANCODE_W])
    mat4_translate(obj, 0, amt, 0, obj);

  if (game->input.keystates[SDL_SCANCODE_S])
    mat4_translate(obj, 0, -amt, 0, obj);

  if (obj == game->test_resources.camera) {
    if (game->input.keystates[SDL_SCANCODE_UP])
      mat4_rotate(obj, amt*0.1, (float[]){1,0,0}, obj);

    if (game->input.keystates[SDL_SCANCODE_RIGHT])
      mat4_rotate(obj, amt*0.1, (float[]){0,1,0}, obj);

    if (game->input.keystates[SDL_SCANCODE_LEFT])
      mat4_rotate(obj, -(amt*0.1), (float[]){0,1,0}, obj);

    if (game->input.keystates[SDL_SCANCODE_DOWN])
      mat4_rotate(obj, -(amt*0.1), (float[]){1,0,0}, obj);
  }

  if (game->input.keystates[SDL_SCANCODE_P])
    mat4_print(obj);
}

void update (struct game *game, u32 dt) {
  static int passed = 0;
  static int last_input = 0;
  static int last_gen_time = 50;
  static float n = 1;
  static double offset = 0.5;
  static int first = 1;
  struct resources *res = &game->test_resources;
  static int stopped = 0;
  struct perlin_settings *ts = &game->terrain->settings;
  float *tnode = game->test_resources.terrain_node;
  float *light = res->light_dir;

  if (first) {
    ts->ox = rand_0to1() * 1000.0;
    ts->oy = rand_0to1() * 1000.0;
    first = 0;
  }

  if (game->input.modifiers & KMOD_LALT)
    movement(game, res->camera);
  else {
    movement(game, res->terrain_node);
    /* movement(game, res->camera); */
  }

  if (game->input.keystates[SDL_SCANCODE_C]) {
    printf("light_dir %f %f %f\n", light[0], light[1], light[2]);

  }

  int space_down = game->input.keystates[SDL_SCANCODE_SPACE];
  int ctrl_down = game->input.modifiers & KMOD_CTRL;
  int now = SDL_GetTicks();
  int dinput = now - last_input;

  if (dinput > 100 && space_down) {
    last_input = SDL_GetTicks();
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

    if (!stopped) {
      for (int i = 12; i < 14; ++i)
        tnode[i] = max(tnode[i], 0);

      tnode[14] = max(tnode[14], 20.0);

      double scale = tnode[14] * 0.01;
      if (scale == 0) scale = 1.0;
      double ox = tnode[12];
      double oy = tnode[13];

      printf("terrain %f %f %f\n", tnode[12], tnode[13], tnode[14]);

      ts->ox = ox;
      ts->oy = oy;

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
      /* free(game->terrain->samples); */

      /* const double pdist = min(5.0, max(1.1, 1.0/scale*1.4)); */

      /* printf("pdist %f\n", pdist); */

      /* struct point *samples = */
      /*   poisson_disk_samples(pdist, game->terrain->size, 30, &game->terrain->n_samples); */

      /* game->terrain->samples = samples; */

      terrain_create(game->terrain);
      int t2 = SDL_GetTicks();
      last_gen_time = t2 - t1;

      n += 0.01f;
    }

    /* res->light_dir[0] = cos(n) * 0.8; */
  }

}
