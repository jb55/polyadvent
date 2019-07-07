
#ifndef MDL_H
#define MDL_H

#include "geometry.h"
#include "model.h"

enum mdl_tag {
    MDL_POSITION = 1,
    MDL_NORMAL,
    MDL_COLOR,
    MDL_INDEX,
    MDL_TEX_COORD,
    MDL_JOINT_IDS,
    MDL_JOINT_WEIGHTS,
    MDL_MIN,
    MDL_MAX,
    N_MDL_TAGS
};

struct mdl_geometry {
    float min[3];
    float max[3];
    struct make_geometry mkgeom;
};


void save_mdl(FILE *out, struct model *model, struct mdl_geometry *geom);
void load_mdl(FILE *in, struct model *model, struct mdl_geometry *geom);

void init_mdl_geometry(struct mdl_geometry *);

#endif /* MDL_H */
