
#include "input.h"
#include "util.h"
#include "common.h"

void input_init(struct input *input) {
  /* memset(input->keys, 0, sizeof(input->keys[0]) * ARRAY_SIZE(input->keys)); */
  input->keystates = SDL_GetKeyboardState(NULL);
  input->is_dragging = 0;
  input->mdx = 0;
  input->mdy = 0;
  assert(input->keystates);
}

void handle_key(struct input *input, SDL_KeyboardEvent key) {
  input->modifiers = SDL_GetModState();
}


void input_reset(struct input *input) {
    input->is_dragging = 0;

    for (int i = 0; i < MOUSE_BUTTONS; ++i)
        input->mbuttons[i] = 0;
}
