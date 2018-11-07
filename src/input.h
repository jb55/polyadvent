

#ifndef POLYADVENT_INPUT_H
#define POLYADVENT_INPUT_H

#include <SDL2/SDL.h>
#include "common.h"

/* enum key_state { */
/*   KEY_NONE, */
/*   KEY_DOWN, */
/*   KEY_UP */
/* }; */

#define MOUSE_BUTTONS 5

struct input {
  /* enum key_state keys[0x7F-0x1F]; */
    u8 const *keystates;
    SDL_Keymod modifiers;
    int mx, my, last_mx, last_my;
    int mdx, mdy;
    float wheel_x, wheel_y;
    int mbuttons[MOUSE_BUTTONS];
};

int input_is_dragging(struct input *input, int mouse_button);

void input_init(struct input *input);

void input_reset(struct input *input);

void handle_key(struct input *input, SDL_KeyboardEvent);

#endif /* POLYADVENT_INPUT_H */
