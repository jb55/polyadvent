
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"

/*
 * Global data used by our render callback:
 */
struct test_resources {
    struct vbo vertex_buffer, element_buffer;
    GLuint vertex_shader, fragment_shader, program;

    struct {
      GLint fade_factor;
    } uniforms;

    struct {
      gpu_addr position;
    } attributes;

    GLfloat fade_factor;
};


struct game_state {
  int counter;
  struct test_resources test_resources;
};


#endif /* PA_GAME_H */
