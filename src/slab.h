
#ifndef SLAB_H
#define SLAB_H

#include "common.h"

struct slab {
  int x;
  int y;
  int z;
  char *voxels;
};

struct slab *
slab_parse(struct slab *slab, void *data);

void
slab_show(const struct slab *slab);

int slab_size(const struct slab *slab);

void
slab_arrays(const struct slab *slab,
            float *verts,
            u16 *indices,
            float *normals);

#endif /* SLAB_H */
