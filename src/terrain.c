
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


double old_noisy_boi(double x, double y) {
  double c = 5.0;
  double h = c;
  return sin(x/c) * cos(y/c) * h;
}

void deriv(double (*noisefn)(double, double), double x, double y,
           double z1, double *dx, double *dy)
{
  static const double h = 0.00001;
  double zx = noisefn(x + h, y);
  double zy = noisefn(x, y - h);
  *dx = (zx - z1)/h;
  *dy = (zy - z1)/h;
}

void
terrain_init(struct terrain *terrain) {
  terrain->width = 1;
  terrain->height = 1;
}

void
terrain_create(struct terrain *terrain) {
  u32 i;
  const double size = 200;
  const int num_verts = size*size;
  float tmp1[3];
  float tmp2[3];
  del_point2d_t *points = calloc(num_verts, sizeof(*points));
  float *zs = calloc(num_verts * 3, sizeof(*zs));
  float *verts = calloc(num_verts * 3, sizeof(*verts));
  float *normals = calloc(num_verts * 3, sizeof(*normals));
  /* int *indices = calloc(num_verts, sizeof(*indices)); */

  // 100 random samples from our noise function
  for (i = 0; i < (u32)num_verts; i++) {
    int n = i*3;
    double dx, dy;
    double x = rand_0to1() * size;
    double y = rand_0to1() * size;
    double z = old_noisy_boi(x, y);
    deriv(old_noisy_boi, x, y, z, &dx, &dy);

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
