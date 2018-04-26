
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
#include <assert.h>
#include <time.h>


int main(void)
{
  struct game game;
  struct slab slab;
  struct geometry slab_geom;
  struct terrain terrain;

  size_t length;

  /* SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES); */
  int width = 640;
  int height = 480;

  SDL_Window *window = SDL_CreateWindow(
    "SDL2/OpenGL Demo", 0, 0, width, height,
    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  srand(time(NULL));

  SDL_GL_CreateContext(window);
  init_gl(&game.test_resources, width, height);
  game_init(&game);
  game.terrain = &terrain;

  struct perlin_settings terrain_settings = {
    .depth = 1,
    .freq  = 0.02,
    .amplitude  = 1.0,
    .exp = 7.3
  };

  terrain_init(&terrain);
  terrain_create(&terrain, &terrain_settings);

  /* slab_buffer = file_contents(SLAB("test.slab"), &length); */
  /* slab_parse(&slab, slab_buffer); */
  /* slab_show(&slab); */
  /* slab_alloc_arrays(&slab, &slab_geom, NULL); */
  /* slab_arrays(&slab, */
  /*             slab_geom.vertices, */
  /*             slab_geom.normals, */
  /*             slab_geom.indices, */
  /*             &slab_geom.num_elements); */

  /* make_buffer_geometry(&slab_geom); */
  // mesh it -> load into vbo

  /* Loop until the user closes the window */


  u32 last = SDL_GetTicks();

  while (1) {
    process_events(game.test_resources.camera_persp, &game.input);
    u32 ticks = SDL_GetTicks();
    update(&game, ticks-last);

    last = ticks;
    render(&game.test_resources, &terrain.geom);

    /* Swap front and back buffers */
    SDL_GL_SwapWindow(window);
  }

  /* free(slab_buffer); */
  /* free_slab_geom(&geom, NULL) */

  //SDL_GL_DeleteContext(gl);
  //return 0;
}

