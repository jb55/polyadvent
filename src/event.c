
#include "event.h"
#include "window.h"

void process_events() {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_WINDOWEVENT_RESIZED:
      handle_resize(event.window.data1, event.window.data2);
      break;
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
    }
  }
}
