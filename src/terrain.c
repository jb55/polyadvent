
#include "terrain.h"


#include "util.h"

//    v6----- v5
//   /|      /|
//  v1------v0|
//  | |     | |
//  | |v7---|-|v4
//  |/      |/
//  v2------v3

void
terrain_init(struct terrain *terrain) {
  terrain->width = 1;
  terrain->height = 1;
}

void
terrain_create(struct terrain *terrain) {
  const int num_verts = 4;
  float *vs;
  float *ns;

  terrain->geom.num_verts = num_verts;
  vs = terrain->geom.vertices = calloc(num_verts * 3, sizeof(*terrain->geom.vertices));
  ns = terrain->geom.normals = calloc(num_verts * 3, sizeof(*terrain->geom.normals));

  for (int i = 0; i < num_verts; ++i) {
  }
}

void
terrain_detroy(struct terrain *terrain) {
  free(terrain->geom.vertices);
  free(terrain->geom.normals);
}

