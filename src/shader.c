
#include <stdlib.h>
#include <stdio.h>

#include "file.h"
#include "gl.h"
#include "debug.h"
#include "shader.h"

GLuint
make_shader(GLenum type, const char *filename) {
  size_t length;
  GLchar *source = (GLchar *)file_contents(filename, &length);
  GLuint shader;
  GLint shader_ok;

  if (!source)
    return 0;

  shader = glCreateShader(type);
  glShaderSource(shader, 1, (const GLchar**)&source, (GLint*)&length);
  free(source);
  glCompileShader(shader);

  glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);

  if (!shader_ok) {
    fprintf(stderr, "Failed to compile %s:\n", filename);
    //show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
    glDeleteShader(shader);
    return 0;
  }

  return shader;
}



GLuint
make_program(GLuint vertex_shader, GLuint fragment_shader) {
  GLint program_ok;
  GLuint program = glCreateProgram();

  if (vertex_shader)
    glAttachShader(program, vertex_shader);

  glAttachShader(program, fragment_shader);
  glLinkProgram(program);

  glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
  if (!program_ok) {
    fprintf(stderr, "Failed to link shader program:\n");
    //show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
    glDeleteProgram(program);
    return 0;
  }
  return program;
}
