
#ifndef PA_GAME_H
#define PA_GAME_H

#include "vbo.h"
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
  SKYBOX_PROGRAM,
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
		GLint ambient_str;
		GLint camera_position;
		GLint depth_mvp;
		GLint depth_vp;
		GLint diffuse_on;
		GLint fog_on;
		GLint light_dir;
		GLint light_intensity;
		GLint model;
		GLint model_view;
		GLint mvp;
		GLint normal_matrix;
		GLint sky_intensity;
		GLint sun_color;
		GLint time;
		GLint view_proj;
	} uniforms;

	struct attributes attributes;

	struct node root;
	entity_id_t player_id;
	struct orbit orbit_camera;
	struct node free_camera;
    const struct node *camera_node;
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
    u64 frame;
    struct user_settings user_settings;
    struct ui ui;
    struct resources test_resources;
    struct input input;
    struct terrain terrain;
};

struct entity *get_player(struct resources *);
struct entity *get_terrain_entity(struct terrain *);
void game_init(struct game *game, int width, int height);
void should_update(struct game *game);
int was_key_pressed_this_frame(struct game *game, int scancode);
int is_free_camera(struct game *game);

#endif /* PA_GAME_H */
