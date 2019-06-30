
#include "model.h"
#include "ply.h"
#include "resource.h"
#include <assert.h>

#define MODELDEF(name) { .id = model_##name, .loaded = 0, .file = #name }

static struct model_def static_models[NUM_STATIC_MODELS] = {
  MODELDEF(tower),
  MODELDEF(icosphere),
  MODELDEF(pirate_officer),
};

static struct resource_manager dyn_modelman;

static int static_models_initialized = 0;

struct model *init_model(struct model *model) {
    init_id(&model->geom_id);
    model->shading = SHADING_VERT_COLOR;
    model->texture = 0;
    return model;
}

struct model *new_model(struct model *model)
{
    init_model(model);
    null_id(&model->geom_id);
    return model;
}

static void initialize_static_models() {
    for (int i = 0; i < NUM_STATIC_MODELS; i++) {
        struct model_def *sm = &static_models[i];
        assert(sm->id == i);
        new_model(&sm->model);
    }
    static_models_initialized = 1;
}

static inline struct model *new_uninitialized_model(model_id *id) {
    return new_resource(&dyn_modelman, id);
}

void init_model_manager() {
    init_resource_manager(&dyn_modelman, sizeof(struct model),
                          DEF_DYNAMIC_MODELS, MAX_DYNAMIC_MODELS);
}


struct model *new_dynamic_model(model_id *id)
{
    return init_model(new_uninitialized_model(id));
}

struct model *get_model(enum static_model m) {
    static char path[128] = {0};

    if (!static_models_initialized)
        initialize_static_models();

    struct model *model = &static_models[m].model;

    if (get_geometry(&model->geom_id))
        return model;

    int ok = 0;

    // Load mesh
    snprintf(path, 128, "data/models/%s.ply", static_models[m].file);
    ok = parse_ply(path, &model->geom_id);
    if (!ok)
        return 0;

    return model;
}
