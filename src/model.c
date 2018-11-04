
#include "model.h"
#include "ply.h"

void init_model(struct model *model) {
    model->shading = SHADING_VERT_COLOR;
    init_geometry(&model->geom);
}

int load_model(struct model *model, const char *name) {
    static char path[128] = {0};

    int ok = 0;

    // Load mesh
    snprintf(path, 128, "data/models/%s.ply", name);
    ok = parse_ply(path, &model->geom);
    if (!ok)
        return 0;

    return 1;
}
