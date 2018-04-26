
#include "input.h"
#include "util.h"
#include "common.h"

void input_init(struct input *input) {
  /* memset(input->keys, 0, sizeof(input->keys[0]) * ARRAY_SIZE(input->keys)); */
  input->keystates = SDL_GetKeyboardState(NULL);
  assert(input->keystates);
}

void handle_key(struct input *input, SDL_KeyboardEvent key) {
  input->modifiers = SDL_GetModState();
}
