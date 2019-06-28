

#ifndef MODEL_H
#define MODEL_H


#include "shader.h"
#include "geometry.h"
#include "common.h"

#define DEF_DYNAMIC_MODELS 128
#define MAX_STATIC_MODELS 128
#define MAX_DYNAMIC_MODELS 2048

typedef struct resource_id model_id;

enum static_model {
  MODEL_TOWER,
  MODEL_ICOSPHERE,
  MODEL_PIRATEOFFICER,
  NUM_STATIC_MODELS
};

enum shading {
    SHADING_TERRAIN,
    SHADING_VERT_COLOR,
    SHADING_UI,
};

struct model {
    geometry_id geom_id;
    enum shading shading;
    u32 texture;
};

struct model_def {
    u16 id;
    int loaded;
    char *file;
    struct model model;
};


void init_model_manager();
struct model *init_model(struct model *model);
struct model *get_model(enum static_model);

struct model *new_dynamic_model(model_id *);

#endif /* MODEL_H */
