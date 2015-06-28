
#ifndef SLAB_H
#define SLAB_H

#include "common.h"

struct slab {
  int x;
  int y;
  int z;
  u8 *voxels;
};

struct slab *
slab_parse(struct slab *slab, void *data);

void
slab_show(const struct slab *slab);

int slab_size(const struct slab *slab);

void
slab_arrays(const struct slab *slab,
            float *verts,
            float *normals,
            u32 *indices,
            int *num_elements
            );

#endif /* SLAB_H */
