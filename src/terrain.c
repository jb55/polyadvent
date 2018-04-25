
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


double old_noisy_boi(double x, double y, double *dx, double *dy) {
  double z = sin(x/5.0) * cos(y/5.0) * 5.0;
  /* *dx =  */
  return z;
}

void
terrain_init(struct terrain *terrain) {
  terrain->width = 1;
  terrain->height = 1;
}

void
terrain_create(struct terrain *terrain) {
  u32 i;
  const int num_verts = 10000;
  del_point2d_t *points = calloc(num_verts, sizeof(*points));
  float *zs = calloc(num_verts * 3, sizeof(*zs));
  float *verts = calloc(num_verts * 3, sizeof(*verts));
  float *normals = calloc(num_verts * 3, sizeof(*normals));
  /* int *indices = calloc(num_verts, sizeof(*indices)); */

  // 100 random samples from our noise function
  for (i = 0; i < num_verts; i++) {
    int n = i*3;
    double dx, dy;
    double x = rand_0to1() * 100.0;
    double y = rand_0to1() * 100.0;
    double z = old_noisy_boi(x, y, &dx, &dy);

    points[i].x = x;
    points[i].y = y;
    zs[i] = z;

    verts[n] = (float)x;
    verts[n+1] = (float)y;
    verts[n+2] = (float)z;

    normals[n] = 0;
    normals[n+1] = 0;
    normals[n+2] = 1;
  }

  delaunay2d_t *del = delaunay2d_from(points, num_verts);
  tri_delaunay2d_t *tri = tri_delaunay2d_from(del);

  printf("faces %d tris %d points %d\n",
         del->num_faces, tri->num_triangles, tri->num_points);

  // TODO: calc normals
  for (i = 0; i < tri->num_points; i++) {
    int n = i*3;

    verts[n]   = (float)tri->points[i].x;
    verts[n+1] = (float)tri->points[i].y;

    normals[n]   = rand_0to1();
    normals[n+1] = rand_0to1();
    normals[n+2] = rand_0to1();
  }

  /* for (i = 0; i < tri->num_triangles; ++i) { */

  /*   int p0 = tri->tris[i * 3 + 0]; */
  /*   int p1 = tri->tris[i * 3 + 1]; */
  /*   int p2 = tri->tris[i * 3 + 2]; */
  /*   vec3 *v0 = (vec3*)&verts[p0]; */
  /*   vec3 *v1 = (vec3*)&verts[p1]; */
  /*   vec3 *v2 = (vec3*)&verts[p2]; */

  /*   float x0 = v0->x; */
  /*   float x1 = v1->x; */
  /*   float x2 = v2->x; */

  /*   float y0 = v0->y; */
  /*   float y1 = v1->y; */
  /*   float y2 = v2->y; */

  /*   float z0 = v0->z; */
  /*   float z1 = v1->z; */
  /*   float z2 = v2->z; */

  /*   float nx = (y1-y0)*(z2-z0)-(y2-y0)*(z1-z0); */
  /*   float ny = (z1-z0)*(x2-x0)-(x1-x0)*(z2-z0); */
  /*   float nz = (x1-x0)*(y2-x0)-(x2-x1)*(y1-y0); */

  /*   normals[p0] = nx; */
  /*   normals[p0+1] = ny; */
  /*   normals[p0+2] = nz; */

  /*   normals[p1] = nx; */
  /*   normals[p1+1] = ny; */
  /*   normals[p1+2] = nz; */

  /*   normals[p2] = nx; */
  /*   normals[p2+1] = ny; */
  /*   normals[p2+2] = nz; */
  /* } */


  terrain->geom.num_verts = num_verts;
  terrain->geom.vertices = (float*)verts;
  terrain->geom.normals = (float*)normals;
  terrain->geom.indices = tri->tris;
  terrain->geom.num_indices = tri->num_triangles * 3;

  make_buffer_geometry(&terrain->geom);

  delaunay2d_release(del);
  tri_delaunay2d_release(tri);

  free(points);
  free(verts);
  free(normals);
}

void
terrain_detroy(struct terrain *terrain) {
  free(terrain->geom.vertices);
  free(terrain->geom.normals);
}
