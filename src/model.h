

#ifndef MODEL_H
#define MODEL_H


#include "shader.h"
#include "geometry.h"
#include "common.h"

#define MAX_MODELS 128

enum shading {
    SHADING_TERRAIN,
    SHADING_VERT_COLOR,
    SHADING_UI,
};

struct model {
    struct geometry geom;
    enum shading shading;
    u32 texture;
};


void init_model(struct model *model);
int load_model(struct model *model, const char *name);


#endif /* MODEL_H */
