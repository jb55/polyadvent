
#include "event.h"
#include "window.h"
#include "input.h"
#include "game.h"

void process_events(struct game *game, float *camera) {
  SDL_Event event;
  int mdx = 0;
  int mdy = 0;

  input_reset(&game->input);

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      handle_key(&game->input, event.key);
      break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button <= MOUSE_BUTTONS)
            game->input.mbuttons[event.button.button-1] = 1;

        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button <= MOUSE_BUTTONS)
            game->input.mbuttons[event.button.button-1] = 0;
        break;
    case SDL_MOUSEMOTION:
        if (event.button.button) {
            /* printf("drag... %d %d\n", event.motion.xrel, event.motion.yrel); */
            game->input.last_mx = game->input.mx;
            game->input.last_my = game->input.mx;
            game->input.mx = event.motion.x;
            game->input.my = event.motion.y;

        }
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
