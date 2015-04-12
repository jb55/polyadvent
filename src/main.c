
#include "gl.h"
#include "game.h"
#include "update.h"
#include "window.h"
#include "event.h"
#include "render.h"

int main(void)
{
  GameState game;

  SDL_Window *window = SDL_CreateWindow(
    "SDL2/OpenGL Demo", 0, 0, 640, 480,
    SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);

  /* SDL_GLContext _SDL_GLContext _gl =gl = */ SDL_GL_CreateContext(window);

  init_gl(&game.test_resources);

  /* Loop until the user closes the window */
  while (1) {
    process_events();
    update(&game);
    render(&game.test_resources);

    /* Swap front and back buffers */
    SDL_GL_SwapWindow(window);
  }

  //SDL_GL_DeleteContext(gl);
  //return 0;
}

