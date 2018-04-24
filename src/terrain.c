
#include "terrain.h"
#include "util.h"
#include "delaunay.h"

static const float plane_verts[] = {
  -1,-1,0,  -1,1,0,  1,1,0,  1,-1,0
};

static const float plane_normals[] = {
  0,0,1,  0,0,1,  0,0,1,  0,0,1
};

static const u32 plane_indices[] = {
  2,1,0,  2,0,3
};


double old_noisy_boi(double x, double y) {
  return sin(x) * cos(y) * 5.0;
}

void
terrain_init(struct terrain *terrain) {
  terrain->width = 1;
  terrain->height = 1;
}

void
terrain_create(struct terrain *terrain) {
  u32 i;
  const int num_verts = 1000;
  del_point2d_t *points = calloc(num_verts, sizeof(*points));
  float *zs = calloc(num_verts * 3, sizeof(*zs));
  float *verts = calloc(num_verts * 3, sizeof(*verts));
  float *normals = calloc(num_verts * 3, sizeof(*normals));
  /* int *indices = calloc(num_verts, sizeof(*indices)); */

  // 100 random samples from our noise function
  for (i = 0; i < num_verts; i++) {
    double x = rand_0to1() * 100.0;
    double y = rand_0to1() * 100.0;

    double z = old_noisy_boi(x, y);

    points[i].x = x;
    points[i].y = y;
    zs[i] = z;

    verts[(i*3)] = (float)x;
    verts[(i*3)+1] = (float)y;
    verts[(i*3)+2] = (float)z;
  }

  delaunay2d_t *del = delaunay2d_from(points, num_verts);
  tri_delaunay2d_t *tri = tri_delaunay2d_from(del);

  // TODO: calc normals
  for (i = 0; i < tri->num_points; i++) {
    /* float y1 =  */
    /* float nx = (y2-y1)*(z3-z1)-(y3-y1)*(z2*-z1); */
    /* float ny = (y2-y1)*(z3-z1)-(y3-y1)*(z2*-z1); */
    int n = i*3;

    verts[n]   = (float)tri->points[i].x;
    verts[n+1] = (float)tri->points[i].y;
    verts[n+2] = 0;

    normals[n]   = 0;
    normals[n+1] = 0;
    normals[n+2] = 1;
  }

  terrain->geom.num_verts = tri->num_triangles * 3;
  terrain->geom.vertices = (float*)verts;
  terrain->geom.normals = (float*)normals;
  terrain->geom.indices = tri->tris;
  terrain->geom.num_indices = tri->num_triangles * 3;

  make_buffer_geometry(&terrain->geom);

  /* delaunay2d_release(del); */
  /* tri_delaunay2d_release(tri); */

  /* free(points); */
  /* free(verts); */
  /* free(normals); */
}

void
terrain_detroy(struct terrain *terrain) {
  free(terrain->geom.vertices);
  free(terrain->geom.normals);
}
