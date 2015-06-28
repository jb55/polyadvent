

#include "slab.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>


#define defconsume(T)                           \
  static inline T                               \
  consume_##T(void **p) {                 \
    T *i = (T*)*p;                            \
    T v = *i++;                                 \
    *p = (void*)i;                              \
    return v;                                   \
  }

defconsume(int)
//defconsume(char)

/* slab_vbo(const void *data) { */
/*   struct slab *slab = (struct slab*)data; */
/*   assert(slab->x == 3); */
/*   assert(slab->y == 3); */
/*   assert(slab->z == 3); */
/* } */

struct slab *slab_parse(struct slab *slab, void *data) {
  void *p = data;
  slab->x = consume_int(&p);
  slab->y = consume_int(&p);
  slab->z = consume_int(&p);
  slab->voxels = (void*)p;
  return slab;
}

int slab_size(const struct slab *slab) {
  return slab->x *
         slab->y *
         slab->z;
}

void slab_show(const struct slab *slab) {
  printf("slab %d %d %d\n", slab->x, slab->y, slab->z);
}



//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

#define BLOCK_AIR 0xFF

static const float cube_verts[] = {
  1, 1, 1,  -1, 1, 1,  -1,-1, 1,   1,-1, 1,    // v0-v1-v2-v3 front
  1, 1,-1,   1, 1, 1,   1,-1, 1,   1,-1,-1,    // v5-v0-v3-v4 right
 -1, 1, 1,   1, 1, 1,   1, 1,-1,  -1, 1,-1,    // v1-v0-v5-v6 top
 -1, 1, 1,  -1, 1,-1,  -1,-1,-1,  -1,-1, 1,    // v1-v6-v7-v2 left
  1,-1, 1,  -1,-1, 1,  -1,-1,-1,   1,-1,-1,    // v3-v2-v7-v4 bottom
 -1, 1,-1,   1, 1,-1,   1,-1,-1,  -1,-1,-1     // v4-v7-v6-v5 back
};

static const float cube_normals[] = {
  0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1, // front
  1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0, // right
  0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0, // top
 -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0, // left
  0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0, // bottom
  0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1  // back
};


static const u32 cube_indices[] = {
   0, 1, 2,   0, 2, 3,    // front
   4, 5, 6,   4, 6, 7,    // right
   8, 9,10,   8,10,11,    // top
  12,13,14,  12,14,15,    // left
  16,17,18,  16,18,19,    // bottom
  20,21,22,  20,22,23
};

void
slab_arrays(const struct slab *slab,
            float *verts,
            float *normals,
            u32 *indices,
            int *num_elements) {
  int x, y, z, i, j;
  u32 start_index = 0;
  int verts_ind, normals_ind, index_ind;
  verts_ind = 0; normals_ind = 0; index_ind = 0;
  int xs = slab->x;
  int ys = slab->y;
  int zs = slab->z;
  int cube_verts_size = ARRAY_SIZE(cube_verts);
  int cube_indices_size = ARRAY_SIZE(cube_indices);
  float n = 0.5f;
  u8 color = 0;

  printf("got here cvs(%d) %d %d %d\n", cube_verts_size, xs, ys, zs);

  for (x = 0; x < xs; ++x)
  for (y = 0; y < ys; ++y)
  for (z = 0; z < zs; ++z) {
    int yind = y * zs;
    int xind = x * zs * ys;
    color = slab->voxels[z + yind + xind];

    /* printf("building color(0x%x) %d %d %d vi(%d) ni(%d) ii(%d)\n", */
    /*        color, x, y, z, verts_ind, normals_ind, index_ind); */

    for (i = 0; i < cube_verts_size; i += 12) // for each face
    for (j = 0; j < 12; j += 3) { // for each vert
      int k = i + j;
      verts[verts_ind++] = cube_verts[k] * n + x;
      verts[verts_ind++] = cube_verts[k+1] * n + y;
      verts[verts_ind++] = cube_verts[k+2] * n + z;

      normals[normals_ind++] = cube_normals[k];
      normals[normals_ind++] = cube_normals[k+1];
      normals[normals_ind++] = cube_normals[k+2];
    }

    // if we have something other than air
    if (color != BLOCK_AIR) {
      for (i = 0; i < cube_indices_size; ++i) {
        indices[index_ind++] = cube_indices[i] + start_index;
      }
    }

    start_index += 24;
  }


  //assert(index_ind < 65535);
  printf("ind %d pos(%d) norm(%d)\n", index_ind, verts_ind, normals_ind);
  printf("ret slab_arrays %d\n", index_ind);
  *num_elements = index_ind;
}
