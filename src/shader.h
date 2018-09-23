#ifndef POLYADVENT_SHADER_H
#define POLYADVENT_SHADER_H

#include <time.h>
#include "gl.h"

#define SHADER(f) "etc/shaders/" f

struct shader {
	GLenum type;
	GLuint handle;
	const char *filename;
#ifdef DEBUG
	time_t load_mtime;
#endif
};

struct gpu_program {
	struct shader vertex;
	struct shader fragment;
	GLuint handle;
};


#ifdef DEBUG
int reload_program(struct gpu_program *program);
#endif

int make_shader(GLenum type, const char *filename, struct shader *shader);

int make_program(struct shader *vertex,
		 struct shader *fragment,
		 struct gpu_program *program);

#endif /* POLYADVENT_SHADER_H */
