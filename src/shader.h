#ifndef POLYADVENT_SHADER_H
#define POLYADVENT_SHADER_H

#include <time.h>
#include "gl.h"

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

struct gpu_program {
	struct shader shaders[MAX_SHADERS];
    int n_shaders;
	GLuint handle;
};

#define NO_GEOM_SHADER NULL

int reload_program(struct gpu_program *program);

int make_shader(GLenum type, const char *filename, struct shader *shader);

int make_program_from_shaders(struct shader **shaders,
                              int n_shaders,
                              struct gpu_program *program);

struct shader *get_program_shader(struct gpu_program *program, GLenum type);

int make_program(struct shader *vertex,
		 struct shader *fragment,
		 struct gpu_program *program);

#endif /* POLYADVENT_SHADER_H */
