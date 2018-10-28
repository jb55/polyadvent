
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"
#include "input.h"
#include "model.h"
#include "node.h"
#include "shader.h"
#include "entity.h"
#include "terrain.h"
#include "ui.h"
#include "fbo.h"

#define PLAYER_HEIGHT 1.73

/*
 * Global data used by our render callback:
 * NOTE: just for testing right now
 */
struct resources {
	struct vbo vertex_buffer, element_buffer, normal_buffer;
    struct fbo shadow_buffer;

	struct gpu_program program;
	struct gpu_program smooth_program;
	struct gpu_program terrain_program;
	struct gpu_program ui_program;

	struct uniforms {
		GLint camera_position;
		GLint light_dir;
		GLint light_intensity;
		GLint mvp;
		GLint normal_matrix;
		GLint view;
		GLint fog_on;
		GLint diffuse_on;
		GLint model_view;
		GLint world;
	} uniforms;

	struct attributes attributes;

	struct node root;
	struct entity player;
	struct node camera;
	struct node sun_camera;

	bool fog_on, diffuse_on;

	float test_mvp[MAT4_ELEMS];
	float light_dir[3];
	float light_intensity[3];
	float proj_persp[MAT4_ELEMS];
	float proj_ortho[MAT4_ELEMS];
};

struct game {
    int counter;
    int seed;
    struct ui ui;
    struct resources test_resources;
    struct input input;
    struct terrain terrain;
};

void game_init(struct game *game, int width, int height);
void should_update(struct game *game);

#endif /* PA_GAME_H */
