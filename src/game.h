
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"
#include "input.h"
#include "node.h"

#define PLAYER_HEIGHT 1.7

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
      GLint local;
      GLint tscale;
      GLint normal_matrix;
    } uniforms;

    struct attributes {
      gpu_addr position;
      gpu_addr normal;
    } attributes;

    struct node player;
    struct node camera;

    float terrain_node[MAT4_ELEMS];
    float normal_matrix[MAT4_ELEMS];
    float test_mvp[MAT4_ELEMS];
    float light_dir[3];
    float camera_persp[MAT4_ELEMS];
    GLfloat fade_factor;
};


struct game {
  int counter;
  struct resources test_resources;
  struct input input;
  struct terrain *terrain;
};

void game_init(struct game *game);
void should_update(struct game *game);

#endif /* PA_GAME_H */
