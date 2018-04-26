
#include "gl.h"
#include "update.h"
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

  if (game->input.keystates[SDL_SCANCODE_UP])
    mat4_rotate(obj, amt*0.1, (float[]){1,0,0}, obj);

  if (game->input.keystates[SDL_SCANCODE_RIGHT])
    mat4_rotate(obj, amt*0.1, (float[]){0,1,0}, obj);

  if (game->input.keystates[SDL_SCANCODE_LEFT])
    mat4_rotate(obj, -(amt*0.1), (float[]){0,1,0}, obj);

  if (game->input.keystates[SDL_SCANCODE_DOWN])
    mat4_rotate(obj, -(amt*0.1), (float[]){1,0,0}, obj);

  if (game->input.keystates[SDL_SCANCODE_P])
    mat4_print(obj);
}

void update (struct game *game, u32 dt) {
  static int passed = 0;
  static float n = 1;
  struct resources *res = &game->test_resources;

  if (game->input.modifiers & KMOD_LALT)
    movement(game, res->camera);
  else {
    movement(game, res->player);
    /* movement(game, res->camera); */
  }

  if (passed < 10) {
    passed += dt;
  } else {
    passed = 0;

    res->light_dir[0] = cos(n) * 0.8;
    n += 0.01f;
  }

}
