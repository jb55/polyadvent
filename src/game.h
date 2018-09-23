
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"
#include "input.h"
#include "node.h"
#include "terrain.h"
#include "shader.h"

#define PLAYER_HEIGHT 1.7

/*
 * Global data used by our render callback:
 * NOTE: just for testing right now
 */
struct resources {
	struct vbo vertex_buffer, element_buffer, normal_buffer;

	struct gpu_program program;

	struct uniforms {
		GLint camera_position;
		GLint light_dir;
		GLint mvp;
		GLint normal_matrix;
		GLint view;
		GLint fog_on;
		GLint diffuse_on;
		GLint model_view;
		GLint world;
	} uniforms;

	struct attributes {
		gpu_addr position;
		gpu_addr normal;
	} attributes;

	struct node root;
	struct node player;
	struct node camera;
	struct node terrain_node;

	bool fog_on, diffuse_on;

	float test_mvp[MAT4_ELEMS];
	float light_dir[3];
	float camera_persp[MAT4_ELEMS];
};

struct game {
  int counter;
  struct resources test_resources;
  struct input input;
  struct terrain terrain;
};

void game_init(struct game *game);
void should_update(struct game *game);

#endif /* PA_GAME_H */
