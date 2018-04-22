

#ifndef POLYADVENT_INPUT_H
#define POLYADVENT_INPUT_H

#include <SDL2/SDL.h>
#include "common.h"

/* enum key_state { */
/*   KEY_NONE, */
/*   KEY_DOWN, */
/*   KEY_UP */
/* }; */

struct input {
  /* enum key_state keys[0x7F-0x1F]; */
  u8 const *keystates;
};

void input_init(struct input *input);

/* void handle_key(struct input *input, SDL_KeyboardEvent *ke); */

#endif /* POLYADVENT_INPUT_H */
