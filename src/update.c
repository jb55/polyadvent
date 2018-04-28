
#include "gl.h"
#include "update.h"
#include "terrain.h"
#include "util.h"
#include "mat4/mat4.h"
#include "vec3/vec3.h"

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
  static int last_gen_time = 50;
  static float n = 1;
  struct resources *res = &game->test_resources;
  int once = 0;
  static struct perlin_settings terrain_settings = {
    .depth = 1,
    .freq  = 0.04,
    .amplitude  = 1.0,
    .ox = 0,
    .oy = 0,
    .o1 = 2.0, .o1s = 0.5,
    .o2 = 4.0, .o2s = 0.25,
    .exp = 7.3
  };
  float *light = res->light_dir;

  if (game->input.modifiers & KMOD_LALT)
    movement(game, res->camera);
  else {
    movement(game, res->player);
    /* movement(game, res->camera); */
  }

  if (game->input.keystates[SDL_SCANCODE_C]) {
    printf("light_dir %f %f %f\n", light[0], light[1], light[2]);

  }

  int space_down = game->input.keystates[SDL_SCANCODE_SPACE];

  if (space_down || passed < last_gen_time) {
    if (space_down && once) {
      printf("terrain amp %f exp %f freq %f (%d ms)\n",
             terrain_settings.amplitude,
             terrain_settings.exp,
             terrain_settings.freq,
             last_gen_time);
    }
    passed += dt;
    once = 0;
  } else {
    once = 1;
    passed = 0;

    terrain_settings.oy += 1.0;
    terrain_settings.ox += 1.0;
    /* terrain_settings.o1s = fabs(sin(1/n) * 0.25); */
    /* terrain_settings.o1 = fabs(cos(n*0.2) * 0.5); */
    /* terrain_settings.o2s = fabs(cos(n+2) * 0.5); */
    /* terrain_settings.o2 = fabs(sin(n*0.02) * 2); */
    /* terrain_settings.freq = fabs(-sin(n)*0.002 * cos(-n)) + 0.02; */
    terrain_settings.exp = fabs(cos(n)*2.0*cos(n)) + 5.0;
    terrain_settings.amplitude = cos(1/n)*2.0*cos(n) + 0.5;

    terrain_destroy(game->terrain);
    terrain_init(game->terrain);

    int t1 = SDL_GetTicks();
    terrain_create(game->terrain, &terrain_settings);
    int t2 = SDL_GetTicks();
    last_gen_time = t2 - t1;

    /* printf("terrain amp %f exp %f freq %f (%d ms)\n", */
    /*        terrain_settings.amplitude, */
    /*        terrain_settings.exp, */
    /*        terrain_settings.freq, */
    /*        last_gen_time); */


    /* res->light_dir[0] = cos(n) * 0.8; */
    n += 0.01f;
  }

}
