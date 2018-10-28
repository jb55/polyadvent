#ifndef POLYADVENT_FBO_H
#define POLYADVENT_FBO_H

#include "common.h"
#include "gl.h"

#define MAX_FBO_ATTACHMENTS 3

struct fbo {
    GLuint handle;
    GLuint attachments[MAX_FBO_ATTACHMENTS];
    int n_attachments;
    int width, height;
};

void init_fbo(struct fbo *fbo);
void bind_fbo(struct fbo *fbo);
void create_fbo(struct fbo *fbo, int width, int height);

int fbo_attach_color_texture(struct fbo *fbo);
int fbo_attach_depth_texture(struct fbo *fbo);

int fbo_attach_texture(struct fbo *fbo, GLint internalformat, GLint format,
                       GLenum attachment, GLenum type);

int fbo_attach_renderbuffer(struct fbo *fbo, GLenum internalformat,
                            GLenum attachment);



void check_fbo(struct fbo *fbo);
void delete_fbo(struct fbo *);
void unbind_fbo(struct fbo *fbo);



#endif /* POLYADVENT_FBO_H */
