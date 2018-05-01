
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

  int seed = time(NULL);
  srand(seed);

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

  SDL_GL_CreateContext(window);
  init_gl(&game.test_resources, width, height);
  game_init(&game);
  game.terrain = &terrain;

  const double size = 1000;
  const double pdist = 1.7;
  const double scale = 0.01;
  /* printf("samples seed %d\n", seed); */
  const int n_samples =
    (size * size) * scale*scale;
  struct point *samples = uniform_samples(n_samples, size);
  terrain.n_samples = n_samples;

  /* struct point *samples = poisson_disk_samples(pdist, size, 30, &terrain.n_samples); */
  /* struct point *samples = load_samples(NULL, &terrain.n_samples); */
  draw_samples(samples, pdist, terrain.n_samples, size);
  /* save_samples(samples, seed, terrain.n_samples); */

  terrain.settings = (struct perlin_settings){
    .depth = 1,
    .freq  = 0.02,
    .o1 = 2.0, .o1s = 0.5,
    .o2 = 4.0, .o2s = 0.25,
    .amplitude  = 1.0,
    .ox = 0,
    .oy = 0,
    .exp = 6.3,
    .scale = 1.0
  };

  terrain_init(&terrain);
  terrain.size = size;
  terrain.samples = samples;
  terrain_create(&terrain);

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
    render(&game, &terrain.geom);

    /* Swap front and back buffers */
    SDL_GL_SwapWindow(window);
  }

  /* free(slab_buffer); */
  /* free_slab_geom(&geom, NULL) */

  //SDL_GL_DeleteContext(gl);
  //return 0;
}

