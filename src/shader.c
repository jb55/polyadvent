
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "file.h"
#include "util.h"
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

static char *cached_file_contents(const char *filename) {
    size_t len;
    for (int i = 0; i < file_buf_count; i++) {
        if (memcmp(file_names[i], filename, file_name_lens[i]) == 0) {
            return file_buffers[i];
        }
    }

    return file_contents(filename, &len);
}

static char *strsep(char **stringp, const char *delim) {
    if (*stringp == NULL) { return NULL; }
    char *token_start = *stringp;
    *stringp = strpbrk(token_start, delim);
    if (*stringp)
        (*stringp)++;
    return token_start;
}

static char **resolve_imports(char *contents, int *nlines, int level) {
    char *line;
    int nline = 0;
    char *resolved_contents;
    char fname_buf[32] = {0};

    while ((line = strsep(&contents, "\n"))) {
        nline++;
        int line_len = contents - line;
        int size = sizeof("#include");
        if (memcmp(line, "#include ", size) == 0) {
            char *filename = line + size;
            int file_name_len = line_len - size - 1;
            snprintf(fname_buf, 32, SHADER("%.*s"), file_name_len, filename);
            file_name_lens[file_buf_count] = file_name_len;
            file_names[file_buf_count] = filename;

            /* printf("got include %.*s at line %d level %d ind %d\n", */
            /*         file_name_len, filename, nline, level, file_buf_count); */

            // TODO (perf): cache file contents based on filename
            resolved_contents = cached_file_contents(fname_buf);
            file_buffers[file_buf_count] = resolved_contents;
            file_buf_count++;
            resolve_imports(resolved_contents, nlines, level + 1);
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


struct shader *get_program_shader(struct gpu_program *program, GLenum type) {
    struct shader *shader;
    for (int i = 0; i < program->n_shaders; i++) {
        shader = &program->shaders[i];
        if (shader->type == type)
            return shader;
    }

    return NULL;
}


int make_shader(GLenum type, const char *filename, struct shader *shader) {
  size_t length;
  int nlines = 0;
  GLchar *source = (GLchar *)file_contents(filename, &length);
  if (!source)
      return 0;

  assert(file_buf_count == 0);
  char **lines = resolve_imports(source, &nlines, 0);

  GLint shader_ok;

  shader->n_includes = 0;
  shader->filename = filename;
  shader->type = type;
  shader->handle = glCreateShader(type);

  glShaderSource(shader->handle, nlines, (const char**)lines,
                 (const int*)line_lens);

  // shader dependency stuff
  for (int i = 0; i < file_buf_count; ++i) {
      assert(i < MAX_SHADER_INCLUDES);
      char *p = shader->includes[shader->n_includes];
      int name_len = file_name_lens[i];
      /* printf("%.*s name len %d\n", name_len, file_names[i], name_len); */
      snprintf(p, MAX_INCLUDE_FNAME_LEN, SHADER("%.*s"), name_len, file_names[i]);
      assert(name_len < MAX_INCLUDE_FNAME_LEN);
      shader->include_mtimes[shader->n_includes] = file_mtime(p);
      /* printf("'%s' included in '%s'\n", p, shader->filename); */
      shader->n_includes++;
      free(file_buffers[i]);
  }
  free(source);

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

void init_gpu_program(struct gpu_program *program) {
    memset(program, 0, sizeof(*program));
}

void add_attribute(struct gpu_program *program, const char *name,
                   enum vertex_attr attr)
{
    program->vertex_attrs[attr] =
        (gpu_addr)glGetAttribLocation(program->handle, name);
    check_gl();
}

void add_uniform(struct gpu_program *program,
                 const char *name,
                 enum uniform_id id)
{
    struct uniform *var = &program->uniforms[id];
    var->name = name;
    var->id = id;
    var->location = glGetUniformLocation(program->handle, var->name);
    if (var->location == -1) {
        debug("warn: could not find uniform location: %s in program %s\n",
              var->name, program->name);
    }
    /* assert(var->location != -1); */
    check_gl();
}

void find_program_uniforms(struct gpu_program *program,
                           struct gpu_program *programs)
{
    // Program variables
    add_uniform(program, "camera_position", UNIFORM_CAMERA_POSITION);
    add_uniform(program, "depth_mvp", UNIFORM_DEPTH_MVP);
    add_uniform(program, "light_intensity", UNIFORM_LIGHT_INTENSITY);
    add_uniform(program, "sky_intensity", UNIFORM_SKY_INTENSITY);
    add_uniform(program, "time", UNIFORM_TIME);
    add_uniform(program, "light_dir", UNIFORM_LIGHT_DIR);
    add_uniform(program, "sun_color", UNIFORM_SUN_COLOR);
    add_uniform(program, "fog_on", UNIFORM_FOG_ON);
    add_uniform(program, "model", UNIFORM_MODEL);
    add_uniform(program, "mvp", UNIFORM_MVP);
    add_uniform(program, "normal_matrix", UNIFORM_NORMAL_MATRIX);

    // Attributes
    add_attribute(program, "normal", va_normal);
    add_attribute(program, "position", va_position);
    add_attribute(program, "color", va_color);

    // chess stuff
    if (program == &programs[CHESS_PIECE_PROGRAM]) {
        add_uniform(program, "is_white",
                    UNIFORM_IS_WHITE);
    }
}

void find_uniforms(struct gpu_program *programs)
{
    for (int i = 0; i < NUM_PROGRAMS; ++i) {
        struct gpu_program *program = &programs[i];
        if (program == NULL) {
            debug("program %d is NULL\n", i);
            continue;
        }

        if (program->name == NULL) {
            debug("program %d name is NULL, not initialized?\n", i);
            continue;
        }

        find_program_uniforms(program, programs);
    }

}

#ifdef DEBUG
int reload_program(struct gpu_program *program,
                   struct gpu_program *programs) {
	int ok;

    int n_shaders = program->n_shaders;
    struct gpu_program new_program;
    struct shader new_shaders[n_shaders];
    struct shader *new_shaders_p[n_shaders];

    init_gpu_program(&new_program);

    int changes[n_shaders];

    for (int i = 0; i < n_shaders; i++) {
        changes[i] = 0;
        struct shader *shader = &program->shaders[i];
        new_shaders_p[i] = shader;

        time_t mtime = file_mtime(shader->filename);
        int changed = mtime != shader->load_mtime;

        for (int j = 0; j < shader->n_includes; ++j) {
            time_t include_mtime = shader->include_mtimes[j];
            time_t new_mtime = file_mtime(shader->includes[j]);
            changed |= include_mtime != new_mtime;
        }
 
        changes[i] = changed;

        if (changed) {
            /* printf("making shader %s\n", shader->filename); */
            ok = make_shader(shader->type, shader->filename, &new_shaders[i]);
            if (!ok) {
                // clean up all the new shaders we've created so far
                for (int k = 0; k < i; k++)
                    glDeleteShader(new_shaders[k].handle);
                return 0;
            }
            new_shaders_p[i] = &new_shaders[i];
        }
    }

    int any_changes = 0;
    for (int i = 0; i < n_shaders; i++)
        any_changes |= changes[i];

    if (!any_changes) {
        return 2;
    }

	ok = make_program_from_shaders(program->name, new_shaders_p,
                                   n_shaders, &new_program);

    if (!ok) {
        for (int i = 0; i < n_shaders; i++) {
            glDeleteShader(program->shaders[i].handle);
        }
        return 0;
    }

    *program = new_program;

    find_program_uniforms(program, programs);
    return 1;
}
#endif

int
make_program(const char *name,
             struct shader *vertex,
             struct shader *fragment,
	         struct gpu_program *program)
{
    struct shader *shaders[] = { vertex, fragment };
    return make_program_from_shaders(name, shaders, 2, program);
}

int make_program_from_shaders(const char *name, struct shader **shaders,
                              int n_shaders, struct gpu_program *program)
{
	GLint program_ok;

	// TODO: relax these constraints
	program->handle = glCreateProgram();
    check_gl();
    program->n_shaders = n_shaders;
    program->name = name;

    assert(n_shaders <= MAX_SHADERS);
    for (int i = 0; i < n_shaders; i++) {
        struct shader *shader = shaders[i];
        program->shaders[i] = *shader;
        /* printf("attaching shader %s\n", shader->filename); */
        /* for (int j = 0; j < shader->n_includes; ++j) { */
        /*     printf("attaching shader %s dep %s \n", shader->filename, */
        /*            shader->includes[j]); */
        /* } */
        glAttachShader(program->handle, shader->handle);
        check_gl();
    }

	glLinkProgram(program->handle);
    check_gl();

	glGetProgramiv(program->handle, GL_LINK_STATUS, &program_ok);

	if (!program_ok) {
		fprintf(stderr, "Failed to link shader program:\n");
		show_info_log(program->handle);
		glDeleteProgram(program->handle);
		return 0;
	}

	return 1;
}

