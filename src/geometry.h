
#include "common.h"

#ifndef GEOMETRY_H
#define GEOMETRY_H


struct geom {
  int num_elements;
  float *vertices;
  float *normals;
  u16 *indices;
};

#endif /* GEOMETRY_H */
