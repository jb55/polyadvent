
#include "terrain.h"
#include "util.h"
#include "delaunay.h"
#include "vec3/vec3.h"

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
  double c = 5.0;
  double h = c;
  double z = sin(x/c) * cos(y/c) * h;
  *dx = (cos(x/c) * cos(y/c) * h)/c;
  *dy = (sin(x/c) * sin(y/c) * h)/c;
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
  const int num_verts = 5000;
  float tmp1[3];
  float tmp2[3];
  float norm[3];
  del_point2d_t *points = calloc(num_verts, sizeof(*points));
  float *zs = calloc(num_verts * 3, sizeof(*zs));
  float *verts = calloc(num_verts * 3, sizeof(*verts));
  float *normals = calloc(num_verts * 3, sizeof(*normals));
  /* int *indices = calloc(num_verts, sizeof(*indices)); */

  // 100 random samples from our noise function
  for (i = 0; i < (u32)num_verts; i++) {
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


    // ^i * dx
    vec3_scale((float[3]){1,0,0}, dx, tmp1);
    // ^k - (^i * dx)
    vec3_subtract((float[3]){0,0,1}, tmp1, tmp2);

    // ^j * dy
    vec3_scale((float[3]){0,1,0}, dy, tmp1);

    // (^k - (^i * dx)) - ^j * dy
    vec3_subtract(tmp2, tmp1, tmp2);

    normals[n] = tmp2[0];
    normals[n+1] = tmp2[1];
    normals[n+2] = tmp2[2];
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

    /* normals[n]   = rand_0to1(); */
    /* normals[n+1] = rand_0to1(); */
    /* normals[n+2] = rand_0to1(); */
  }

  /* for (i = 0; i < tri->num_triangles; ++i) { */

  /*   int p0 = tri->tris[i * 3 + 0]; */
  /*   int p1 = tri->tris[i * 3 + 1]; */
  /*   int p2 = tri->tris[i * 3 + 2]; */

  /*   vec3 *v0 = (vec3*)&verts[p0]; */
  /*   vec3 *v1 = (vec3*)&verts[p1]; */
  /*   vec3 *v2 = (vec3*)&verts[p2]; */

  /*   vec3_subtract(v1, v0, tmp1); */
  /*   vec3_subtract(v2, v0, tmp2); */
  /*   vec3_cross(tmp1, tmp2, norm); */

  /*   normals[p0] = norm[0]; */
  /*   normals[p0+1] = norm[1]; */
  /*   normals[p0+2] = norm[2]; */

  /*   normals[p1] = norm[0]; */
  /*   normals[p1+1] = norm[1]; */
  /*   normals[p1+2] = norm[2]; */

  /*   normals[p2] = norm[0]; */
  /*   normals[p2+1] = norm[1]; */
  /*   normals[p2+2] = norm[2]; */
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
