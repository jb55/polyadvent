
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"

/*
 * Global data used by our render callback:
 */
struct test_resources {
    struct vbo vertex_buffer, element_buffer, normal_buffer;
    GLuint vertex_shader, fragment_shader, program;

    struct {
      GLint fade_factor;
      GLint light_dir;
      GLint mvp;
      GLint normal_matrix;
    } uniforms;

    struct {
      gpu_addr position;
      gpu_addr normal;
    } attributes;

    float normal_matrix[MAT4_ELEMS];
    float test_mvp[MAT4_ELEMS];
    GLfloat fade_factor;
};


struct game_state {
  int counter;
  struct test_resources test_resources;
};

void init_game(struct game_state *game);

#endif /* PA_GAME_H */
