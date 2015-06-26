
#ifndef SLAB_GEOM
#define SLAB_GEOM

#include "geometry.h"
#include "slab.h"
#include <stddef.h>

struct geom_t*
alloc_slab_geom(const struct slab_t *slab,
                struct geom_t *geom,
                void* (*_realloc)(void*, size_t));

void
free_slab_geom(struct geom_t *geom,
               void (*_free)(void*));

#endif // SLAB_GEOM
