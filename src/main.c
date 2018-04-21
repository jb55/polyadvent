
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


int main(void)
{
  struct game_state game;
  struct slab slab;
  struct geometry slab_geom;
  struct terrain terrain;

  size_t length;
  void *slab_buffer;

  SDL_Window *window = SDL_CreateWindow(
    "SDL2/OpenGL Demo", 0, 0, 640, 480,
    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  SDL_GL_CreateContext(window);

  init_gl(&game.test_resources);
  init_game(&game);

  terrain_init(&terrain);
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
  while (1) {
    process_events();
    update(&game);
    render(&game.test_resources, &slab_geom);

    /* Swap front and back buffers */
    SDL_GL_SwapWindow(window);
  }

  /* free(slab_buffer); */
  /* free_slab_geom(&geom, NULL) */

  //SDL_GL_DeleteContext(gl);
  //return 0;
}

