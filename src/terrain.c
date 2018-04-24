
#include "terrain.h"
#include "util.h"

static const float plane_verts[] = {
  -1,-1,0,  -1,1,0,  1,1,0,  1,-1,0
};

static const float plane_normals[] = {
  0,0,1,  0,0,1,  0,0,1,  0,0,1
};

static const u32 plane_indices[] = {
  2,1,0,  2,0,3
};

void
terrain_init(struct terrain *terrain) {
  terrain->width = 1;
  terrain->height = 1;
}

void
terrain_create(struct terrain *terrain) {
  const int num_verts = 4;

  terrain->geom.num_verts = num_verts;
  terrain->geom.vertices = (float*)plane_verts;
  terrain->geom.normals = (float*)plane_normals;
  terrain->geom.indices = (u32*)plane_indices;
  terrain->geom.num_indices = ARRAY_SIZE(plane_indices);

  make_buffer_geometry(&terrain->geom);
}

void
terrain_detroy(struct terrain *terrain) {
  free(terrain->geom.vertices);
  free(terrain->geom.normals);
}


float old_noisy_boi(float x, float y) {
  return sinf(x) * cosf(y);
}
