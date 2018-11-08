
#ifndef PA_GAME_H
#define PA_GAME_H

#include "buffer.h"
#include "entity.h"
#include "fbo.h"
#include "input.h"
#include "model.h"
#include "node.h"
#include "orbit.h"
#include "shader.h"
#include "skybox.h"
#include "terrain.h"
#include "texture.h"
#include "ui.h"

#define PLAYER_HEIGHT 1.73

#define MAX_PROGRAMS 12

enum program_type {
  DEFAULT_PROGRAM,
  TERRAIN_PROGRAM,
  UI_PROGRAM,
  NUM_PROGRAMS,
};

/*
 * Global data used by our render callback:
 * NOTE: just for testing right now
 */
struct resources {
	struct vbo vertex_buffer, element_buffer, normal_buffer;
    struct fbo shadow_buffer;

    struct gpu_program programs[NUM_PROGRAMS];

	struct uniforms {
		GLint camera_position;
		GLint light_dir;
		GLint light_intensity;
		GLint sky_intensity;
		GLint ambient_str;
		GLint sun_color;
		GLint mvp;
		GLint depth_mvp;
		GLint depth_vp;
		GLint normal_matrix;
		GLint view;
		GLint time;
		GLint fog_on;
		GLint diffuse_on;
		GLint model_view;
		GLint world;
	} uniforms;

	struct attributes attributes;

	struct node root;
	struct entity_id player_id;
	struct orbit camera;
	struct node sun_camera;

    u32 test_cubemap;
    float time;
	bool fog_on, diffuse_on;

    struct skybox skybox;
	float sun_color[3];
	float test_mvp[MAT4_ELEMS];
	float light_dir[3];
	float light_intensity;
	float proj_persp[MAT4_ELEMS];
	float proj_ortho[MAT4_ELEMS];
};

struct user_settings {
    float mouse_sens;
};

struct game {
    int counter;
    int seed;
    float dt;
    struct user_settings user_settings;
    struct ui ui;
    struct resources test_resources;
    struct input input;
    struct terrain terrain;
};

struct entity *get_player(struct resources *);
void game_init(struct game *game, int width, int height);
void should_update(struct game *game);

#endif /* PA_GAME_H */
