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

void bind_fbo(struct fbo *fbo);
void create_fbo(struct fbo *fbo, int width, int height);
int fbo_attach_texture(struct fbo *, GLenum attachment);
void fbo_check(struct fbo *fbo);
int fbo_attach_renderbuffer(struct fbo *fbo, GLenum internalformat, GLenum attachment);
void delete_fbo(struct fbo *);
void unbind_fbo(struct fbo *fbo);



#endif /* POLYADVENT_FBO_H */
