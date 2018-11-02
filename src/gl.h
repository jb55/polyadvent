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

void glDeleteBuffers(GLsizei n, const GLuint * buffers);

void glVertexAttribPointer(GLuint index, GLint size, GLenum type,
                           GLboolean normalized, GLsizei stride,
                           const GLvoid * pointer);

void glUniform1i(GLint location, GLint i);
void glUniform3f(GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
GLuint glCreateProgram(void);
void glAttachShader(GLuint program, GLuint shader);
void glLinkProgram(GLuint program);

void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose,
                        const GLfloat *value);

void glUseProgram(GLuint program);
void glUniform1f(GLint location, GLfloat v0);
void glUniform2f(GLint location, GLfloat v0, GLfloat v1);
void glGetProgramiv(GLuint program, GLenum pname, GLint *params);
void glDeleteProgram(	GLuint program);

void glGenFramebuffers(GLsizei handle, GLuint *ids);
void glBindFramebuffer(	GLenum target, GLuint framebuffer);
GLenum glCheckFramebufferStatus(GLenum target);
void glDeleteFramebuffers(GLsizei n, const GLuint * framebuffers);

void glDisableVertexAttribArray(GLuint index);
void glDeleteVertexArrays(GLsizei n, const GLuint *arrays);

GLboolean glIsBuffer(	GLuint buffer);


void glTexImage2D(	GLenum target,
                    GLint level,
                    GLint internalformat,
                    GLsizei width,
                    GLsizei height,
                    GLint border,
                    GLenum format,
                    GLenum type,
                    const GLvoid * data);

void glFramebufferTexture2D(	GLenum target,
                                GLenum attachment,
                                GLenum textarget,
                                GLuint texture,
                                GLint level);

void glGenRenderbuffers(	GLsizei n,
                            GLuint * renderbuffers);

void glBindRenderbuffer(	GLenum target,
                            GLuint renderbuffer);

void glRenderbufferStorage(	GLenum target,
                            GLenum internalformat,
                            GLsizei width,
                            GLsizei height);

void glFramebufferRenderbuffer(	GLenum target,
                                GLenum attachment,
                                GLenum renderbuffertarget,
                                GLuint renderbuffer);


void glDeleteRenderbuffers(	GLsizei n,
                            GLuint *renderbuffers);


void glFramebufferTexture(	GLenum target,
                            GLenum attachment,
                            GLuint texture,
                            GLint level);

void glPatchParameteri(	GLenum pname,
                        GLint value);

#endif /* POLYADVENT_GL_H */
