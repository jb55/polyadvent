
#include "terrain.h"
#include "util.h"
#include "delaunay.h"
#include "vec3/vec3.h"
#include "perlin.h"
#include "poisson.h"

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
  /* return cos(x/10.0) * sin(y/10.0) * 20.0; */
  struct terrain *t = (struct terrain*)data;
  struct perlin_settings *s = &t->settings;
  x *= s->scale;
  y *= s->scale;
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
}

void
terrain_create(struct terrain *terrain) {
  u32 i;
  const double size = terrain->size;
  const double hsize = size;
  int num_verts = hsize*hsize;
  float tmp1[3], tmp2[3];
  del_point2d_t *points = calloc(num_verts, sizeof(*points));
  struct perlin_settings *perlin = &terrain->settings;
  float *verts = calloc(num_verts * 3, sizeof(*verts));
  float *normals = calloc(num_verts * 3, sizeof(*normals));
  double ox = perlin->ox * (1/perlin->scale);
  double oy = perlin->oy * (1/perlin->scale);

  // 100 random samples from our noise function
  for (i = 0; i < (u32)terrain->n_samples; i++) {
    int n = i*3;
    double x, y;

    x = terrain->samples[i].x;
    y = terrain->samples[i].y;

    double z = old_noisy_boi((void*)terrain, ox+x, oy+y);

    points[i].x = x;
    points[i].y = y;

    verts[n] = (float)x;
    verts[n+1] = (float)y;

    static const double limit = 1.4;
    if (x < limit || x > size-limit || y < limit || y > size-limit)
      verts[n+2] = 0;
    else
      verts[n+2] = (float)z;
  }

  delaunay2d_t *del = delaunay2d_from(points, terrain->n_samples);
  tri_delaunay2d_t *tri = tri_delaunay2d_from(del);

  num_verts = tri->num_triangles * 3;
  float *del_verts = calloc(num_verts * 3, sizeof(*del_verts));
  float *del_norms = calloc(num_verts * 3, sizeof(*del_norms));
  u32   *del_indices = calloc(num_verts, sizeof(*del_indices));

  /// XXX (perf): we should be able to do this directly from del instead of
  /// triangulating with tri
  for (i = 0; i < tri->num_triangles; ++i) {
    int nv = i * 3;
    int ndv = i * 9;

    int p[3] = {
      tri->tris[nv + 0],
      tri->tris[nv + 1],
      tri->tris[nv + 2],
    };

    float *v[3] = {
      &verts[p[0]*3],
      &verts[p[1]*3],
      &verts[p[2]*3]
    };

    del_verts[ndv+0] = v[0][0];
    del_verts[ndv+1] = v[0][1];
    del_verts[ndv+2] = v[0][2];

    del_verts[ndv+3] = v[1][0];
    del_verts[ndv+4] = v[1][1];
    del_verts[ndv+5] = v[1][2];

    del_verts[ndv+6] = v[2][0];
    del_verts[ndv+7] = v[2][1];
    del_verts[ndv+8] = v[2][2];

    vec3_subtract(v[1], v[0], tmp1);
    vec3_subtract(v[2], v[0], tmp2);
    vec3_cross(tmp1, tmp2, tmp2);
    vec3_normalize(tmp2, tmp2);

    for (int j = 0; j < 9; ++j) {
      del_norms[ndv+j] = tmp2[j%3];
    }

    del_indices[nv+0] = nv+0;
    del_indices[nv+1] = nv+1;
    del_indices[nv+2] = nv+2;
  }

  /* printf("faces %d tris %d points %d\n", */
  /*        del->num_faces, tri->num_triangles, tri->num_points); */

  terrain->geom.num_verts = num_verts;
  terrain->geom.vertices = (float*)del_verts;
  terrain->geom.normals = (float*)del_norms;
  terrain->geom.indices = (u32*)del_indices;
  terrain->geom.num_indices = num_verts;

  make_buffer_geometry(&terrain->geom);

  delaunay2d_release(del);
  tri_delaunay2d_release(tri);

  free(points);
  free(verts);
  free(normals);
  free(del_verts);
  free(del_norms);
  free(del_indices);
}


void terrain_destroy(struct terrain *terrain) {
  destroy_buffer_geometry(&terrain->geom);
}
