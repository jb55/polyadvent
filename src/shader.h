#ifndef POLYADVENT_SHADER_H
#define POLYADVENT_SHADER_H

#include <time.h>
#include "gl.h"
#include "vbo.h"

#define SHADER(f) "etc/shaders/" f

#define MAX_SHADER_INCLUDES 16
#define MAX_INCLUDE_FNAME_LEN 64
#define MAX_SHADERS 5

struct shader {
	GLenum type;
	GLuint handle;
    int n_includes;
	const char *filename;
    char includes[MAX_SHADER_INCLUDES][MAX_INCLUDE_FNAME_LEN];
    time_t include_mtimes[MAX_SHADER_INCLUDES];
	time_t load_mtime;
};

enum program_type {
    DEFAULT_PROGRAM,
    TERRAIN_PROGRAM,
    UI_PROGRAM,
    SKYBOX_PROGRAM,
    CHESS_PIECE_PROGRAM,
    NUM_PROGRAMS,
};


enum uniform_id {
    UNIFORM_AMBIENT_STR,
    UNIFORM_CAMERA_POSITION,
    UNIFORM_DEPTH_MVP,
    UNIFORM_DEPTH_VP,
    UNIFORM_DIFFUSE_ON,
    UNIFORM_FOG_ON,
    UNIFORM_LIGHT_DIR,
    UNIFORM_LIGHT_INTENSITY,
    UNIFORM_MODEL,
    UNIFORM_MODEL_VIEW,
    UNIFORM_MVP,
    UNIFORM_NORMAL_MATRIX,
    UNIFORM_SKY_INTENSITY,
    UNIFORM_SUN_COLOR,
    UNIFORM_TIME,
    UNIFORM_VIEW_PROJ,
    UNIFORM_IS_WHITE,
    MAX_UNIFORMS
};

struct uniform {
    enum uniform_id id;
    const char *name;
    int location;
};

struct gpu_program {
	struct shader shaders[MAX_SHADERS];
    struct uniform uniforms[MAX_UNIFORMS];
	gpu_addr vertex_attrs[MAX_VERTEX_ATTRS];
    int n_shaders;
	GLuint handle;
    const char *name;
};

void add_uniform(struct gpu_program *program, const char *name, enum uniform_id id);
void add_attribute(struct gpu_program *program, const char *name, enum vertex_attr attr);

#define NO_GEOM_SHADER NULL

int reload_program(struct gpu_program *program, struct gpu_program *programs);
int make_shader(GLenum type, const char *filename, struct shader *shader);

void find_uniforms(struct gpu_program *programs);
void find_program_uniforms(struct gpu_program *program,
                           struct gpu_program *programs);
void init_gpu_program(struct gpu_program *program);
int make_program_from_shaders(const char *name,
                              struct shader **shaders,
                              int n_shaders,
                              struct gpu_program *program);

struct shader *get_program_shader(struct gpu_program *program, GLenum type);

int make_program(const char *name, struct shader *vertex, struct shader *fragment, struct gpu_program *program);

#endif /* POLYADVENT_SHADER_H */
