
#include "gl.h"
#include "update.h"
#include "terrain.h"
#include "util.h"
#include "mat4/mat4.h"

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
  static struct perlin_settings terrain_settings = {
    .depth = 1,
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

  if (passed < last_gen_time) {
    passed += dt;
  } else {
    passed = 0;

    terrain_settings.depth = 1;

    terrain_settings.amplitude = sin(n);
    terrain_settings.exp = fabs(sin(n) * 10.0);
    terrain_settings.freq = fabs(cos(n) * 0.04);


    terrain_destroy(game->terrain);
    terrain_init(game->terrain);

    int t1 = SDL_GetTicks();
    terrain_create(game->terrain, &terrain_settings);
    int t2 = SDL_GetTicks();
    last_gen_time = t2 - t1;

    printf("terrain amp %f exp %f freq %f (%d ms)\n",
           terrain_settings.amplitude,
           terrain_settings.exp,
           terrain_settings.freq,
           last_gen_time);

    /* res->light_dir[0] = cos(n) * 0.8; */
    n += 0.01f;
  }

}
