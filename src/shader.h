#ifndef POLYADVENT_SHADER_H
#define POLYADVENT_SHADER_H

#define SHADER(f) "etc/shaders/" f

GLuint make_shader(GLenum type, const char *filename);
GLuint make_program(GLuint vertex_shader, GLuint fragment_shader);

#endif /* POLYADVENT_SHADER_H */
