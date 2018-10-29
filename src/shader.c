
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "gl.h"
#include "debug.h"
#include "shader.h"
#include "common.h"

#define MAX_LINES 4096

static int file_buf_count = 0;
static char *file_buffers[12];
static char *line_buff[MAX_LINES];
static u32 line_lens[MAX_LINES];

static char *strsep(char **stringp, const char *delim) {
    if (*stringp == NULL) { return NULL; }
    char *token_start = *stringp;
    *stringp = strpbrk(token_start, delim);
    if (*stringp)
        (*stringp)++;
    return token_start;
}

static char **resolve_imports(char *contents, int *nlines) {
    char *line;
    size_t file_len;
    char *resolved_contents;
    static char fname_buf[32];

    while ((line = strsep(&contents, "\n"))) {
        int line_len = contents - line;
        int size = sizeof("#include");
        if (memcmp(line, "#include ", size) == 0) {
            char *filename = line + size;
            snprintf(fname_buf, 32, SHADER("%.*s"), line_len-size-1, filename);
            /* printf("got include %s\n", fname_buf); */
            resolved_contents = file_contents(fname_buf, &file_len);
            file_buffers[file_buf_count++] = resolved_contents;
            resolve_imports(resolved_contents, nlines);
        }
        else {
            line_buff[*nlines] = line;
            line_lens[*nlines] = line_len;
            (*nlines)++;
        }
    }

    return line_buff;
}

int make_shader(GLenum type, const char *filename, struct shader *shader) {
  size_t length;
  int nlines = 0;
  GLchar *source = (GLchar *)file_contents(filename, &length);
  if (!source)
      return 0;

  char **lines = resolve_imports(source, &nlines);

  GLint shader_ok;

  shader->filename = filename;
  shader->type = type;
  shader->handle = glCreateShader(type);

  glShaderSource(shader->handle, nlines, (const char**)lines,
                 (const int*)line_lens);

  free(source);
  for (int i = 0; i < file_buf_count; ++i)
      free(file_buffers[i]);
  file_buf_count = 0;

  glCompileShader(shader->handle);

  glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &shader_ok);

  if (!shader_ok) {
    fprintf(stderr, "Failed to compile %s:\n", filename);
    show_info_log(shader->handle);
    glDeleteShader(shader->handle);
    return 0;
  }

#ifdef DEBUG
  shader->load_mtime =
	file_mtime(shader->filename);
#endif

  return 1;
}


#ifdef DEBUG
int reload_program(struct gpu_program *program) {
	struct shader vert, frag;
	struct shader *pvert, *pfrag;
	time_t frag_mtime, vert_mtime;
	int ok;

	vert_mtime = file_mtime(program->vertex.filename);
	frag_mtime = file_mtime(program->fragment.filename);

	int vert_changed =
		vert_mtime != program->vertex.load_mtime;

	int frag_changed =
		frag_mtime != program->fragment.load_mtime;

	if (!vert_changed && !frag_changed)
		return 2;

	// recompile vertex shader
	if (vert_changed) {
		glDeleteShader(program->vertex.handle);

		ok =
			make_shader(GL_VERTEX_SHADER,
				    program->vertex.filename,
				    &vert);

		if (!ok)
			return 0;
	}

	// recompile fragment shader
	if (frag_changed) {
		glDeleteShader(program->fragment.handle);

		ok =
			make_shader(GL_FRAGMENT_SHADER,
				    program->fragment.filename,
				    &frag);

		if (!ok)
			return 0;
	}

	if (vert_changed) {
		glDeleteShader(program->vertex.handle);
		pvert = &vert;
		vert.load_mtime = vert_mtime;
	}
	else
		pvert = &program->vertex;

	if (frag_changed) {
		glDeleteShader(program->fragment.handle);
		pfrag = &frag;
		frag.load_mtime = frag_mtime;
	}
	else
		pfrag = &program->fragment;

	glDeleteProgram(program->handle);

	make_program(pvert, pfrag, program);

	return 1;
}
#endif

int
make_program(struct shader *vertex, struct shader *fragment,
	     struct gpu_program *program)
{
	GLint program_ok;

	// TODO: relax these constraints
	assert(vertex);
	assert(fragment);

	program->handle =
		glCreateProgram();

	program->fragment = *fragment;
	program->vertex = *vertex;

	glAttachShader(program->handle, vertex->handle);
	glAttachShader(program->handle, fragment->handle);

	glLinkProgram(program->handle);

	glGetProgramiv(program->handle, GL_LINK_STATUS, &program_ok);

	if (!program_ok) {
		fprintf(stderr, "Failed to link shader program:\n");
		show_info_log(program->handle);
		glDeleteProgram(program->handle);
		return 0;
	}

	return 1;
}
