
#include "model.h"
#include "ply.h"
#include <assert.h>

#define MODELDEF(name) { .id = model_##name, .loaded = 0, .file = #name }

static struct model_def static_models[NUM_STATIC_MODELS] = {
  MODELDEF(tower),
  MODELDEF(icosphere),
  MODELDEF(pirateofficer),
};

static int static_models_initialized = 0;

void init_model(struct model *model) {
    model->shading = SHADING_VERT_COLOR;
    model->texture = 0;
    init_geometry_id(&model->geom_id);
}

static void initialize_static_models() {
    for (int i = 0; i < NUM_STATIC_MODELS; i++) {
        struct model_def *sm = &static_models[i];
        assert(sm->id == i);
        init_model(&sm->model);
    }
    static_models_initialized = 1;
}

struct model *get_model(enum static_model m) {
    static char path[128] = {0};

    if (!static_models_initialized)
        initialize_static_models();

    struct model *model = &static_models[m].model;

    if (model->geom_id.index >= 0)
        return model;

    int ok = 0;

    // Load mesh
    snprintf(path, 128, "data/models/%s.ply", static_models[m].file);
    ok = parse_ply(path, &model->geom_id);
    if (!ok)
        return 0;

    return model;
}
