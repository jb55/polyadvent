

#ifndef MODEL_H
#define MODEL_H


#include "shader.h"
#include "geometry.h"
#include "common.h"
#include "animation.h"
#include "static_resources.h"

#define DEF_DYNAMIC_MODELS 128
#define MAX_STATIC_MODELS 128
#define MAX_DYNAMIC_MODELS 2048
#define MAX_POSES 2

typedef struct resource_id model_id;

enum shading {
    SHADING_TERRAIN,
    SHADING_VERT_COLOR,
    SHADING_UI,
};

struct model {
    /* geometry_id geom_id; */
    geometry_id geom_id;
    enum shading shading;
    struct pose poses[MAX_POSES]; // TODO: separate animated_model buffer?
    int nposes;
    u32 texture;
};

struct model_def {
    u16 id;
    int loaded;
    char *file;
};

void init_model_manager();
void init_model_id(model_id *id);
struct model *init_model(struct model *model);
struct model *get_model(model_id *);
struct model *get_model_geom(model_id *, struct geometry **);
struct model *new_model(model_id *);
struct model *new_static_model(model_id *);
void destroy_model(model_id *);

model_id get_static_model(enum static_model, struct model**);

#endif /* MODEL_H */
