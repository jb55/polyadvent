
#include "event.h"
#include "window.h"
#include "input.h"
#include "game.h"

void process_events(struct ui *ui, float *camera, struct input *input) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      handle_key(input, event.key);
      break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
        handle_resize(camera, event.window.data1, event.window.data2);
        resize_ui(ui, event.window.data1, event.window.data2);
        break;
      }
      break;
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
    }
  }
}
