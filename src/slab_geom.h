
#ifndef SLAB_GEOM
#define SLAB_GEOM

#include "geometry.h"
#include "slab.h"
#include <stddef.h>

struct geom*
alloc_slab_geom(const struct slab *slab,
                struct geom *geom,
                void* (*_realloc)(void*, size_t));

void
free_slab_geom(struct geom *geom,
               void (*_free)(void*));

#endif // SLAB_GEOM
