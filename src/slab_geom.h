
#ifndef SLAB_GEOM
#define SLAB_GEOM

#include "geometry.h"
#include "slab.h"
#include <stddef.h>

struct geometry*
slab_alloc_arrays(const struct slab *slab,
                  struct geometry *geom,
                  void* (*_realloc)(void*, size_t));

void
slab_free_arrays(struct geometry *geom,
                 void (*_free)(void*));

#endif // SLAB_GEOM
