
#include "gl.h"
#include "game.h"
#include "slab.h"
#include "file.h"
#include "update.h"
#include "window.h"
#include "slab_geom.h"
#include "slab.h"
#include "geometry.h"
#include "event.h"
#include "render.h"
#include "terrain.h"
#include "util.h"
#include <assert.h>
#include <time.h>
#include "poisson.h"
#include "uniform.h"


int main(void)
{
  int nsamples;

  /* int seed = time(NULL); */
  srand(42);

  struct game game;

  /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); */
  int width = 640;
  int height = 480;

  SDL_Window *window = SDL_CreateWindow(
    "SDL2/OpenGL Demo", 0, 0, width, height,
    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  SDL_GL_CreateContext(window);
  init_gl(&game.test_resources, width, height);
  game_init(&game);
  u32 last = SDL_GetTicks();

  while (1) {
    process_events(game.test_resources.camera_persp, &game.input);
    u32 ticks = SDL_GetTicks();
    update(&game, ticks-last);

    last = ticks;
    render(&game, &game.terrain.geom);

    /* Swap front and back buffers */
    SDL_GL_SwapWindow(window);
  }

  /* free(slab_buffer); */
  /* free_slab_geom(&geom, NULL) */

  //SDL_GL_DeleteContext(gl);
  //return 0;
}

