
#include "slab_geom.h"
#include <stdlib.h>
#include <assert.h>

struct geometry*
slab_alloc_arrays(const struct slab *slab,
                  struct geometry *geom,
                  void* (*_realloc)(void*, size_t)) {
  _realloc = _realloc ? _realloc : realloc;
  float *verts, *normals;
  u32 *indices;
  const int num_cubes = slab_size(slab);
  const int num_sides = 6;
  const int vert_per_side = 4;
  const int wut_factor = 0;
  const int num_verts = (num_cubes * num_sides * vert_per_side) + wut_factor;
  const size_t verts_allocd  = (size_t)num_verts * 3 * sizeof(float);
  printf("stuff ncubes(%d) num_verts(%d) num_sides(%d) vert_per_side(%d) allocd(%zu)\n",
         num_cubes,
         num_verts,
         num_sides,
         vert_per_side,
         verts_allocd 
         );
  verts = malloc(verts_allocd);
  normals = malloc(verts_allocd);
  indices = malloc((size_t)num_verts * sizeof(u32));

  assert(verts);
  assert(normals);
  assert(indices);

  geom->vertices = verts;
  geom->indices = indices;
  geom->normals = normals;

  geom->num_verts = num_verts - wut_factor;

  return geom;
}


void
slab_free_arrays(struct geometry *geom,
                 void (*_free)(void*)) {
  _free = _free ? _free : free;
  _free(geom->vertices);
  _free(geom->indices);
  _free(geom->normals);
}
