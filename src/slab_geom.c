
#include "slab_geom.h"
#include <stdlib.h>

struct geom*
alloc_slab_geom(const struct slab *slab,
                struct geom *geom,
                void* (*_realloc)(void*, size_t)) {
  _realloc = _realloc ? _realloc : realloc;
  float *verts, *normals;
  u16 *indices;
  const int num_cubes = slab_size(slab);
  const int num_sides = 6;
  const int vert_per_side = 4;
  const int num_verts = num_cubes * num_sides * vert_per_side;
  verts = _realloc(NULL, (size_t)num_verts * 3 * sizeof(float));
  normals = _realloc(NULL, (size_t)num_verts * 3 * sizeof(float));
  indices = _realloc(NULL, (size_t)num_verts * sizeof(u16));
  geom->vertices = verts;
  geom->indices = indices;
  geom->normals = normals;
  geom->num_elements = num_verts;
  return geom;
}


void
free_slab_geom(struct geom_t *geom,
               void (*_free)(void*)) {
  _free = _free ? _free : free;
  _free(geom->vertices);
  _free(geom->indices);
  _free(geom->normals);
}
