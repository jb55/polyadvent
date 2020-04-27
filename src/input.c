
#include "input.h"
#include "util.h"
#include "debug.h"
#include "common.h"

static void key_down(struct input *input, int scancode, u64 current_frame) {
    input->modifiers = SDL_GetModState();

    struct input_edge *edge = &input->key_edge_states[scancode];

    if (edge->is_down)
        return;

    edge->down_frame = current_frame;
    edge->is_down = 1;

    // might be useful to know when it was last up
    /* edge->up_frame = 0; */
}

static void key_up(struct input *input, int scancode, u64 current_frame) {
    struct input_edge *edge = &input->key_edge_states[scancode];

    edge->up_frame = current_frame;
    edge->is_down = 0;
}

static void button_up(struct input *input, SDL_JoyButtonEvent *event, u64 current_frame)
{
    if (event->button >= SDL_CONTROLLER_BUTTON_MAX) return;
    /* printf("button up %d\n", event->button); */
    struct input_edge *edge = &input->button_edge_states[event->button];

    edge->up_frame = current_frame;
    edge->is_down = 0;
}

static void button_down(struct input *input, SDL_JoyButtonEvent *event, u64 current_frame)
{
    if (event->button >= SDL_CONTROLLER_BUTTON_MAX) return;
    printf("button down %d\n", event->button);

    struct input_edge *edge = &input->button_edge_states[event->button];

    if (edge->is_down)
        return;

    edge->down_frame = current_frame;
    edge->is_down = 1;
}

static void axis_motion(struct input *input, SDL_JoyAxisEvent *event)
{
    if (event->axis >= MAX_AXIS) return;
    input->axis[event->axis] = event->axis == 1 ? -event->value : event->value;
    /* printf("axis %d %d", input->axis[0], input->axis[1]); */
    for (int i = 0; i < MAX_AXIS; i++) {
        if (input->axis[i] >= -4000 && input->axis[i] <= 4000 )
            input->axis[i] = 0;
    }
    /* printf(" -> %d %d\n", input->axis[0], input->axis[1]); */
}

void process_events(struct input *input, u64 current_frame) {
  SDL_Event event;

  input->last_mx = input->mx;
  input->last_my = input->my;

  input_reset(input);

  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    case SDL_MOUSEWHEEL:
        input->wheel_x = event.wheel.x;
        input->wheel_y = event.wheel.y;
        break;
    case SDL_KEYDOWN:
        key_down(input, event.key.keysym.scancode, current_frame);
        break;
    case SDL_KEYUP:
        key_up(input, event.key.keysym.scancode, current_frame);
        break;
    case SDL_JOYAXISMOTION:
        axis_motion(input, &event.jaxis);
        break;
    case SDL_JOYBUTTONUP:
        button_up(input, &event.jbutton, current_frame);
        break;
    case SDL_JOYBUTTONDOWN:
        button_down(input, &event.jbutton, current_frame);
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (event.button.button <= MOUSE_BUTTONS)
            input->mbuttons[event.button.button-1] = 1;

        break;
    case SDL_MOUSEBUTTONUP:
        if (event.button.button <= MOUSE_BUTTONS)
            input->mbuttons[event.button.button-1] = 0;
        break;
    case SDL_MOUSEMOTION:
        input->mx = event.motion.x;
        input->my = event.motion.y;
        input->mdx += event.motion.xrel;
        input->mdy += event.motion.yrel;
        break;
    case SDL_WINDOWEVENT:
      switch (event.window.event) {
      case SDL_WINDOWEVENT_RESIZED:
          if (!event.window.data1)
              continue;
          debug("resizing %d %d\n", event.window.data1, event.window.data2);
          input->resized_width = event.window.data1;
          input->resized_height = event.window.data2;
          assert(input->resized_width);
          assert(input->resized_height);
        break;
      }
      break;
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
    }

  }

  /* if (input->resized_width) */
  /*   printf("checking resize %d %d\n", input->resized_width, */
  /*           input->resized_height); */

}

void init_input(struct input *input) {
  /* memset(input->keys, 0, sizeof(input->keys[0]) * ARRAY_SIZE(input->keys)); */
  input->keystates = SDL_GetKeyboardState(NULL);
  assert(sizeof(input->key_edge_states) == SDL_NUM_SCANCODES * sizeof(input->key_edge_states[0]));
  memset(input->key_edge_states, 0, sizeof(input->key_edge_states));
  memset(input->button_edge_states, 0, sizeof(input->button_edge_states));
  memset(input->axis, 0, sizeof(input->axis));
  input->axis_min_input = 1024;
  input->mx = 0;
  input->my = 0;
  input->mdx = 0;
  input->mdy = 0;
  input->wheel_x = 0;
  input->wheel_y = 0;
  input->last_mx = 0;
  input->last_my = 0;
  input->resized_height = 0;
  input->resized_width = 0;
  input->controller = 0;
  assert(input->keystates);
}

bool is_key_down_on_frame(struct input *input, u8 scancode, u64 frame) {
    struct input_edge *edge = &input->key_edge_states[scancode];

    // is_down is implied, but do it for good measure
    return edge->down_frame == frame && edge->is_down;
}

bool is_button_down_on_frame(struct input *input, SDL_GameControllerButton button, u64 frame)
{
    struct input_edge *edge = &input->button_edge_states[button];
    return edge->down_frame == frame && edge->is_down;
}


void input_reset(struct input *input) {
    input->mdx = 0;
    input->mdy = 0;
    input->wheel_x = 0;
    input->wheel_y = 0;
    input->resized_height = 0;
    input->resized_width = 0;
}

int input_is_dragging(struct input *input, int mouse_button) {
    return input->mbuttons[mouse_button-1];
}

bool is_button_down(struct input *input, SDL_GameControllerButton button)
{
    if (!input->controller) {
        return false;
    }

    return SDL_GameControllerGetButton(input->controller, button) == 1;
}
