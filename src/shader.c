
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
static char *file_buffers[MAX_SHADER_INCLUDES];
static char *file_names[MAX_SHADER_INCLUDES];
static int file_name_lens[MAX_SHADER_INCLUDES];
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
            int file_name_len = line_len - size - 1;
            snprintf(fname_buf, 32, SHADER("%.*s"), file_name_len, filename);
            file_name_lens[file_buf_count] = file_name_len;
            file_names[file_buf_count] = filename;
            /* printf("got include %s\n", fname_buf); */
            // TODO (perf): cache file contents based on filename
            resolved_contents = file_contents(fname_buf, &file_len);
            file_buffers[file_buf_count++] = resolved_contents;
            resolve_imports(resolved_contents, nlines);
        }
        else {
            line_buff[*nlines] = line;
            line_lens[*nlines] = line_len;
            (*nlines)++;
            /* printf("%d %.*s", *nlines, line_len, line); */
        }
    }

    return line_buff;
}

int make_shader(GLenum type, const char *filename, struct shader *shader) {
  size_t length;
  int nlines = 0;
  static char fname_buf[32];
  GLchar *source = (GLchar *)file_contents(filename, &length);
  if (!source)
      return 0;

  char **lines = resolve_imports(source, &nlines);

  GLint shader_ok;

  shader->n_includes = 0;
  shader->filename = filename;
  shader->type = type;
  shader->handle = glCreateShader(type);

  glShaderSource(shader->handle, nlines, (const char**)lines,
                 (const int*)line_lens);

  free(source);

  // shader dependency stuff
  for (int i = 0; i < file_buf_count; ++i) {
      assert(i < MAX_SHADER_INCLUDES);
      char *p = shader->includes[shader->n_includes];
      int name_len = file_name_lens[i];
      assert(name_len < MAX_INCLUDE_FNAME_LEN);
      snprintf(p, MAX_INCLUDE_FNAME_LEN, SHADER("%.*s"), name_len, file_names[i]);
      shader->include_mtimes[shader->n_includes] = file_mtime(p);
      /* printf("including %s as dep of %s\n", p, name_len, shader->filename); */
      shader->n_includes++;
      free(file_buffers[i]);
  }
  file_buf_count = 0;

  glCompileShader(shader->handle);

  glGetShaderiv(shader->handle, GL_COMPILE_STATUS, &shader_ok);

  if (!shader_ok) {
    fprintf(stderr, "Failed to compile %s:\n", filename);
    show_info_log(shader->handle);
    glDeleteShader(shader->handle);
    return 0;
  }

  shader->load_mtime =
	file_mtime(shader->filename);

  return 1;
}

#define N_SHADERS 3

#ifdef DEBUG
int reload_program(struct gpu_program *program) {
	struct shader vert, frag;
	struct shader *pvert, *pfrag;
	time_t frag_mtime, vert_mtime;
	int ok;

    int n_shaders = program->n_shaders;
    struct shader new_shaders[n_shaders];
    struct shader *new_shaders_p[n_shaders];

    int changes[n_shaders];

    for (int i = 0; i < n_shaders; i++) {
        new_shaders_p[i] = &new_shaders[i];
        changes[i] = 0;
        struct shader *shader = &program->shaders[i];

        if (!shader)
            continue;

        time_t mtime = file_mtime(shader->filename);
        int changed = mtime != shader->load_mtime;

        for (int j = 0; i < shader->n_includes; ++j) {
            time_t include_mtime = shader->include_mtimes[j];
            time_t new_mtime = file_mtime(shader->includes[j]);
            changed |= mtime != new_mtime;
        }
 
        changes[i] = changed;

        if (changed) {
            GLuint old_handle = shader->handle;

            ok = make_shader(shader->type, shader->filename, &new_shaders[i]);
            if (!ok) {
                // clean up all the new shaders we've created so far
                for (int k = 0; k < i; k++)
                    glDeleteShader(new_shaders[k].handle);
                return 0;
            }
        }
    }

    int any_changes = 0;
    for (int i = 0; i < n_shaders; i++)
        any_changes |= changes[i];

    if (!any_changes)
        return 2;

    // delete old shaders
    for (int i = 0; i < n_shaders; i++) {
        glDeleteShader(program->shaders[i].handle);
    }

	glDeleteProgram(program->handle);

    // TODO: cleanup failed make_program?
	return make_program_from_shaders(new_shaders_p, n_shaders, program);
}
#endif

int
make_program(struct shader *vertex,
             struct shader *fragment,
	         struct gpu_program *program)
{
    struct shader *shaders[] = { vertex, fragment };
    return make_program_from_shaders(shaders, 2, program);
}

int
make_program_from_shaders(struct shader **shaders, int n_shaders, struct gpu_program *program)
{
	GLint program_ok;

	// TODO: relax these constraints
	program->handle = glCreateProgram();

    assert(n_shaders <= MAX_SHADERS);
    for (int i = 0; i < n_shaders; i++) {
        program->shaders[i] = *shaders[i];
        struct shader *shader = &program->shaders[i];

        glAttachShader(program->handle, shader->handle);
    }

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
