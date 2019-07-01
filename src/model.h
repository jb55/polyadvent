

#ifndef MODEL_H
#define MODEL_H


#include "shader.h"
#include "geometry.h"
#include "common.h"

#define DEF_DYNAMIC_MODELS 128
#define MAX_STATIC_MODELS 128
#define MAX_DYNAMIC_MODELS 2048

enum static_model {
  model_tower,
  model_icosphere,
  model_pirate_officer,
  NUM_STATIC_MODELS
};

enum model_type {
  STATIC_MODEL,
  DYNAMIC_MODEL
};

typedef struct model_id_t
{
    enum model_type type;
    union {
        struct resource_id dyn_model_id;
        enum static_model static_model_id;
    };
} model_id;


enum shading {
    SHADING_TERRAIN,
    SHADING_VERT_COLOR,
    SHADING_UI,
};

struct model {
    /* geometry_id geom_id; */
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
void init_model_id(model_id *id);
struct model *init_model(struct model *model);
struct model *get_model(model_id *);
struct model *get_model_geom(model_id *, struct geometry **);
struct model *new_model(model_id *);
struct model *new_static_model(model_id *);

model_id get_static_model(enum static_model, struct model**);

#endif /* MODEL_H */
