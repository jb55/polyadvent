
#include "terrain.h"
#include "util.h"
#include "delaunay.h"
#include "vec3/vec3.h"
#include "perlin.h"

static const float plane_verts[] = {
  -1,-1,0,  -1,1,0,  1,1,0,  1,-1,0
};

static const float plane_normals[] = {
  0,0,1,  0,0,1,  0,0,1,  0,0,1
};

static const u32 plane_indices[] = {
  2,1,0,  2,0,3
};


double old_noisy_boi(void *data, double x, double y) {
  struct perlin_settings *s = (struct perlin_settings*)data;
  double e =  perlin2d(x, y, s->freq, s->depth)
            + s->o1s * perlin2d(s->o1 * x, s->o1 * y, s->freq, s->depth)
            + s->o2s * perlin2d(s->o2 * x, s->o2 * y, s->freq, s->depth);
  return pow(e, s->exp) * s->amplitude;
}

void deriv(double (*noisefn)(void*, double, double), void* data, double x,
           double y, double z1, double *dx, double *dy)
{
  static const double h = 0.01;
  double zx = noisefn(data, x + h, y);
  double zy = noisefn(data, x, y - h);
  *dx = (zx - z1)/h;
  *dy = (zy - z1)/h;
}

void
terrain_init(struct terrain *terrain) {
  terrain->width = 1;
  terrain->height = 1;
}

void
terrain_create(struct terrain *terrain, struct perlin_settings *perlin) {
  u32 i;
  const double size = 200;
  const double hsize = size/2;
  const int num_verts = hsize*hsize;
  static int first = 1;
  static float samples_x[40000];
  static float samples_y[40000];
  float tmp2[3];
  del_point2d_t *points = calloc(num_verts, sizeof(*points));
  float *verts = calloc(num_verts * 3, sizeof(*verts));
  float *normals = calloc(num_verts * 3, sizeof(*normals));
  double ox = perlin->ox;
  double oy = perlin->oy;

  // 100 random samples from our noise function
  for (i = 0; i < (u32)num_verts; i++) {
    int n = i*3;
    double dx, dy, x, y;

    if (first) {
      x = rand_0to1() * size;
      y = rand_0to1() * size;
      samples_x[i] = x;
      samples_y[i] = y;
    }
    else {
      x = samples_x[i];
      y = samples_y[i];
    }

    double z = old_noisy_boi((void*)perlin, ox+x, oy+y);
    deriv(old_noisy_boi, (void*)perlin, ox+x, oy+y, z, &dx, &dy);

    points[i].x = x;
    points[i].y = y;

    verts[n] = (float)x;
    verts[n+1] = (float)y;
    verts[n+2] = (float)z;

    // ^k - (^i * dx)
    vec3_subtract(V3(0,0,1), V3(dx,0,0), tmp2);

    // (^k - (^i * dx)) - ^j * dy
    vec3_subtract(tmp2, V3(0,dy,0), tmp2);
    vec3_normalize(tmp2, tmp2);

    normals[n] = tmp2[0];
    normals[n+1] = tmp2[1];
    normals[n+2] = tmp2[2];
  }

  first = 0;


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
terrain_destroy(struct terrain *terrain) {
  destroy_buffer_geometry(&terrain->geom);
}
