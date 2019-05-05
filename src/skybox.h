
#ifndef SKYBOX_H
#define SKYBOX_H

#include "model.h"
#include "shader.h"
#include "node.h"
#include "mat4.h"

struct skybox {
    struct gpu_program *program;
    struct model model;
    gpu_addr attrs[n_vertex_attrs];
    struct node node;
    struct {
        int mvp;
    } uniforms;
};

void create_skybox(struct skybox *skybox, struct gpu_program *program);
void render_skybox(struct skybox *skybox, mat4 *camera);

#endif /* SKYBOX_H */
