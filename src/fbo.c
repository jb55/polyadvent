
#include <assert.h>
#include "fbo.h"
#include "util.h"

void create_fbo(struct fbo *fbo, int width, int height) {
    glGenFramebuffers(1, &fbo->handle);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    check_gl();

    fbo->n_attachments = 0;
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

void init_fbo(struct fbo *fbo) {
    fbo->n_attachments = 0;
    fbo->handle = 0;
    fbo->width = 0;
    fbo->height = 0;
}

int fbo_attach_texture(struct fbo *fbo, GLint internalformat, GLint format,
                       GLenum attachment, GLenum type) {
    assert(fbo->n_attachments < MAX_FBO_ATTACHMENTS);
    GLuint *texture = &fbo->attachments[fbo->n_attachments++];

    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    glGenTextures(1, texture);
    check_gl();
    glBindTexture(GL_TEXTURE_2D, *texture);
    check_gl();
    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, fbo->width, fbo->height, 0,
                 format, type, NULL);
    check_gl();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    check_gl();
    if (attachment == GL_DEPTH_ATTACHMENT) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    check_gl();

    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, *texture,
                           0);

    check_gl();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_gl();

    return *texture;
}

void check_fbo(struct fbo *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    check_gl();
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_gl();
}

void delete_fbo(struct fbo *fbo) {
    // TODO: delete attachments

    glDeleteFramebuffers(1, &fbo->handle);
    check_gl();
}


void bind_fbo(struct fbo *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo->handle);
    check_gl();
}

void unbind_fbo(struct fbo *fbo) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    check_gl();
}

int fbo_attach_depth_texture(struct fbo *fbo) {
    return fbo_attach_texture(fbo, GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT,
                              GL_DEPTH_ATTACHMENT, GL_FLOAT);
}

int fbo_attach_color_texture(struct fbo *fbo) {
    return fbo_attach_texture(fbo, GL_RGB, GL_RGB, GL_COLOR_ATTACHMENT0,
                              GL_UNSIGNED_BYTE);
}
