
#ifndef DAE_H
#define DAE_H

#include "geometry.h"
#include "model.h"

void load_model(const char *filename, struct model *model,
                struct make_geometry *geom);

#endif /* DAE_H */
