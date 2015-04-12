
#include "gl.h"
#include "update.h"

void update (GameState * game) {
  unsigned int ms = SDL_GetTicks();

  // Update fade effect in shader
  game->test_resources.fade_factor = sinf((float)ms * 0.001f) * 0.5f + 0.5f;
}
