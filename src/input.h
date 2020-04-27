

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

/* #define KEY_BUFFER_SIZE 32 */

#define FLAG_KEY_DOWN (1<<0)
#define FLAG_KEY_UP   (1<<1)

#define MAX_AXIS 2
#define MAX_AXIS_VALUE 32767
#define MIN_AXIS_VALUE -32767
#define MAX_CONTROLLERS

struct input_edge {
    int is_down;
    u64 down_frame;
    u64 up_frame;
};

struct input {
  /* enum key_state keys[0x7F-0x1F]; */
    u8 const *keystates;
    SDL_Keymod modifiers;
    SDL_GameController *controller;
    int mx, my, last_mx, last_my;
    int axis[MAX_AXIS];
    int axis_min_input;
    int mdx, mdy;
    float wheel_x, wheel_y;
    int mbuttons[MOUSE_BUTTONS];
    int n_frame_down_keys;
    int n_frame_up_keys;
    int resized_width;
    int resized_height;
    /* u64 down_key_frames[KEY_BUFFER_SIZE]; */
    /* u64 up_key_frames[KEY_BUFFER_SIZE]; */
    /* u8 frame_down_keys[KEY_BUFFER_SIZE]; */
    /* u8 frame_up_keys[KEY_BUFFER_SIZE]; */
    struct input_edge key_edge_states[SDL_NUM_SCANCODES];
    struct input_edge button_edge_states[SDL_CONTROLLER_BUTTON_MAX];
};

int input_is_dragging(struct input *input, int mouse_button);

bool is_key_down_on_frame(struct input *input, u8 scancode, u64 frame);
bool is_button_down_on_frame(struct input *input, SDL_GameControllerButton button, u64 frame);

void input_init(struct input *input);

void input_reset(struct input *input);

void handle_key(struct input *input, SDL_KeyboardEvent);

void process_events(struct input *input, u64 current_frame);

#endif /* POLYADVENT_INPUT_H */
