

#ifndef MODEL_H
#define MODEL_H


#include "shader.h"
#include "geometry.h"
#include "common.h"

#define MAX_STATIC_MODELS 128

enum static_model {
  model_tower,
  model_icosphere,
  model_pirateofficer,
  NUM_STATIC_MODELS
};

enum shading {
    SHADING_TERRAIN,
    SHADING_VERT_COLOR,
    SHADING_UI,
};

struct model {
    struct geometry_id geom_id;
    enum shading shading;
    u32 texture;
};

struct model_def {
    u16 id;
    int loaded;
    char *file;
    struct model model;
};


void init_model(struct model *model);
struct model *get_model(enum static_model);


#endif /* MODEL_H */
