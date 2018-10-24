

#ifndef MODEL_H
#define MODEL_H


#include "shader.h"
#include "geometry.h"
#include "common.h"

struct model {
    struct geometry geom;
    u32 program;
};


int load_model(struct model *model, const char *name);


#endif /* MODEL_H */
