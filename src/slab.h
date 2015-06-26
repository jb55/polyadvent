
#ifndef SLAB_H
#define SLAB_H

#include "common.h"

struct slab_t {
  int x;
  int y;
  int z;
  char *voxels;
};

struct slab_t *
slab_parse(struct slab_t *slab, void *data);

void
slab_show(const struct slab_t *slab);

int slab_size(const struct slab_t *slab);

void
slab_arrays(const struct slab_t *slab,
            float *verts,
            u16 *indices,
            float *normals);

#endif /* SLAB_H */
