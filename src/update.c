
#include "gl.h"
#include "update.h"
#include "mat4/mat4.h"

void camera_movement(struct game *game) {
  float *camera = game->test_resources.camera;
  float amt = 0.1;

  if (game->input.keystates[SDL_SCANCODE_A]) {
    mat4_translate(camera, -amt, 0, 0, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_D]) {
    mat4_translate(camera, amt, 0, 0, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_W]) {
    mat4_translate(camera, 0, amt, 0, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_S]) {
    mat4_translate(camera, 0, -amt, 0, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_UP]) {
    /* mat4_translate(camera, 0, 0, 0.1, camera); */
    /* mat4_translate(camera, 0, -amt, 0, camera); */
    mat4_rotate(camera, amt*0.1, (float[]){1,0,0}, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_RIGHT]) {
    /* mat4_translate(camera, amt, 0, 0, camera); */
    /* mat4_translate(camera, amt, 0, 0, camera); */
    mat4_rotate(camera, amt*0.1, (float[]){0,1,0}, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_LEFT]) {
    /* mat4_translate(camera, -amt, 0, 0, camera); */
    /* mat4_translate(camera, -amt, 0, 0, camera); */
    mat4_rotate(camera, -(amt*0.1), (float[]){0,1,0}, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_DOWN]) {
    /* mat4_translate(camera, -0.1, 0, 0, camera); */
    /* mat4_translate(camera, 0, amt, 0, camera); */
    mat4_rotate(camera, -(amt*0.1), (float[]){1,0,0}, camera);
  }

  if (game->input.keystates[SDL_SCANCODE_P]) {
    mat4_print(camera);
  }
}

void update (struct game *game) {
  unsigned int ms = SDL_GetTicks();
  camera_movement(game);
}
