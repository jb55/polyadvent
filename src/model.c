
#include "model.h"
#include "ply.h"
#include "resource.h"
#include "debug.h"
#include "util.h"
#include <assert.h>

#define MODELDEF(name) { .id = model_##name, .loaded = 0, .file = #name }

static struct model_def static_model_defs[NUM_STATIC_MODELS] = {
  MODELDEF(tower),
  MODELDEF(icosphere),
  MODELDEF(pirate_officer),
};

static struct resource_manager dyn_modelman;

struct model *init_model(struct model *model) {
    init_id(&model->geom_id);
    model->shading = SHADING_VERT_COLOR;
    model->texture = 0;
    return model;
}

static inline struct model *static_models()
{
    return (struct model *)dyn_modelman.resources;
}


static void initialize_static_models() {
    struct model *models = static_models();
    for (int i = 0; i < NUM_STATIC_MODELS; i++) {
        struct model *sm = &models[i];
        init_model(sm);
    }
}

static inline struct model *new_uninitialized_model(struct resource_id *id) {
    return new_resource(&dyn_modelman, id);
}

static struct model *new_model_resource(model_id *model_id)
{
    struct model *model = new_uninitialized_model(model_id);
    /* debug("new model %llu\n", model_id->dyn_model_id.uuid); */
    init_id(&model->geom_id);
    new_geometry(&model->geom_id);
    return model;
}

void init_model_manager()
{
    init_resource_manager(&dyn_modelman, sizeof(struct model),
                          DEF_DYNAMIC_MODELS, MAX_DYNAMIC_MODELS, "model",
                          NUM_STATIC_MODELS);

    initialize_static_models();
}


struct model *new_model(model_id *id)
{
    return new_model_resource(id);
}


struct model *get_model(model_id *model_id)
{
    return get_resource(&dyn_modelman, model_id);
}

static struct model *load_static_model(enum static_model m)
{
    static char path[128] = {0};

    struct model *model = &static_models()[m];

    if (is_id_allocated(&model->geom_id) && get_geometry(&model->geom_id))
        return model;

    int ok = 0;

    // Load mesh
    snprintf(path, 128, "data/models/%s.ply", static_model_defs[m].file);
    ok = parse_ply(path, &model->geom_id);
    if (!ok)
        return 0;

    return model;
}

void destroy_model(model_id *model_id)
{
    if (is_static_resource(model_id))
        return;

    struct model *model = get_model(model_id);

    destroy_geometry(&model->geom_id);
    destroy_resource(&dyn_modelman, model_id);
}


model_id get_static_model(enum static_model m, struct model **model)
{
    if (model)
        *model = load_static_model(m);

    return make_static_id(m);
}


