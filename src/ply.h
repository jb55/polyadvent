

#ifndef PLYPARSER_H
#define PLYPARSER_H

#include "geometry.h"

int parse_ply(const char *filename, geometry_id_t *geom_id);

#endif /* PLYPARSER_H */
