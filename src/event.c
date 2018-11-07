
#include "event.h"
#include "window.h"
#include "input.h"
#include "game.h"

void process_events(struct game *game, float *camera) {
  SDL_Event event;
  int mdx = 0;
  int mdy = 0;

  struct input *input = &game->input;

  input->last_mx = game->input.mx;
  input->last_my = game->input.my;

  input_reset(&game->input);

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_MOUSEWHEEL:
        input->wheel_x = event.wheel.x;
        input->wheel_y = event.wheel.y;
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      handle_key(input, event.key);
      break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button <= MOUSE_BUTTONS)
            input->mbuttons[event.button.button-1] = 1;

        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button <= MOUSE_BUTTONS)
            input->mbuttons[event.button.button-1] = 0;
        break;
    case SDL_MOUSEMOTION:
        input->mx = event.motion.x;
        input->my = event.motion.y;
        input->mdx += event.motion.xrel;
        input->mdy += event.motion.yrel;
        break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        handle_resize(game, event.window.data1, event.window.data2);
        break;
      }
      break;
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
    }

  }

}
