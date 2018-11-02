
#include "input.h"
#include "util.h"
#include "common.h"

void input_init(struct input *input) {
  /* memset(input->keys, 0, sizeof(input->keys[0]) * ARRAY_SIZE(input->keys)); */
  input->keystates = SDL_GetKeyboardState(NULL);
  input->is_dragging = 0;
  input->mx = 0;
  input->my = 0;
  input->last_mx = 0;
  input->last_my = 0;
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

int input_is_dragging(struct input *input, int mouse_button) {
    return input->mbuttons[mouse_button-1] &&
        (input->last_mx != input->mx || input->last_my != input->my);
}
