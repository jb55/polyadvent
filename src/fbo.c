
#include <assert.h>
#include "fbo.h"

void create_fbo(struct fbo *fbo, int width, int height) {
    glGenFramebuffers(1, &fbo->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);

    fbo->width = width;
    fbo->height = height;
}

int fbo_attach_renderbuffer(struct fbo *fbo,
                            GLenum internalformat, GLenum attachment) {
    assert(fbo->n_attachments < MAX_FBO_ATTACHMENTS);
    GLuint *rbo = &fbo->attachments[fbo->n_attachments++];

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    glGenRenderbuffers(1, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, *rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, fbo->width, fbo->height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, *rbo);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return *rbo;
}

int fbo_attach_texture(struct fbo *fbo, GLenum attachment) {
    assert(fbo->n_attachments < MAX_FBO_ATTACHMENTS);
    GLuint *texture = &fbo->attachments[fbo->n_attachments++];

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    glGenTextures(1, texture);
    glBindTexture(GL_TEXTURE_2D, *texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo->width, fbo->height, 0, GL_RGB,
                  GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, *texture,
                           0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return *texture;
}

void fbo_check(struct fbo *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void delete_fbo(struct fbo *fbo) {
    glDeleteFramebuffers(1, &fbo->handle);
}


void bind_fbo(struct fbo *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
}

void unbind_fbo(struct fbo *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
