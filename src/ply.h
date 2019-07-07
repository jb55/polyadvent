

#ifndef PLYPARSER_H
#define PLYPARSER_H

#include "mdl.h"

int parse_ply(const char *filename, struct geometry *geom);
int parse_ply_with_mkgeom(const char *filename, struct mdl_geometry *mdlgeom);

#endif /* PLYPARSER_H */
