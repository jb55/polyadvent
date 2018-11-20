

#ifndef PLYPARSER_H
#define PLYPARSER_H

#include "geometry.h"

int parse_ply(const char *filename, struct geometry_id *geom_id);

#endif /* PLYPARSER_H */
