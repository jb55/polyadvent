#ifndef POLYADVENT_GL_H
#define POLYADVENT_GL_H

/* #include <epoxy/gl.h> */
#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glext.h>

void glGenBuffers(GLsizei n, GLuint * buffers);
void glGetShaderiv(GLuint shader, GLenum pname, GLint *params);
void glGetShaderInfoLog(GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
void glBindBuffer(GLenum target, GLuint buffer);
GLuint glCreateShader(GLenum shaderType);
void glCompileShader(GLuint shader);

void glShaderSource(GLuint shader, GLsizei count, const GLchar **string,
                    const GLint *length);

void glBufferData(GLenum target, GLsizeiptr size, const GLvoid * data, GLenum usage);
GLint glGetUniformLocation(GLuint program, const GLchar *name);
GLint glGetAttribLocation(GLuint program, const GLchar *name);
void glEnableVertexAttribArray(GLuint index);
void glDeleteShader(GLuint shader);

void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const GLvoid * pointer);

void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLuint glCreateProgram(void);
void glAttachShader(GLuint program, GLuint shader);
void glLinkProgram(GLuint program);

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose,
                        const GLfloat *value);
 
void glUseProgram(GLuint program);
void glUniform1f(GLint location, GLfloat v0);
void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
void glDeleteProgram(	GLuint program);
 


#endif /* POLYADVENT_GL_H */
