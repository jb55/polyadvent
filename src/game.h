
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"
#include "input.h"

/*
 * Global data used by our render callback:
 * NOTE: just for testing right now
 */
struct resources {
    struct vbo vertex_buffer, element_buffer, normal_buffer;
    GLuint vertex_shader, fragment_shader, program;

    struct uniforms {
      GLint fade_factor;
      GLint light_dir;
      GLint mvp;
      GLint normal_matrix;
    } uniforms;

    struct attributes {
      gpu_addr position;
      gpu_addr normal;
    } attributes;

    float normal_matrix[MAT4_ELEMS];
    float test_mvp[MAT4_ELEMS];
    float camera[MAT4_ELEMS];
    float camera_persp[MAT4_ELEMS];
    GLfloat fade_factor;
};


struct game {
  int counter;
  struct resources test_resources;
  struct input input;
};

void game_init(struct game *game);

#endif /* PA_GAME_H */
