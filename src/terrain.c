
#include "terrain.h"
#include "util.h"
#include "delaunay.h"
#include "vec3.h"
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


double old_noisy_boi(struct terrain *t, double x, double y) {
  /* return cos(x/10.0) * sin(y/10.0) * 20.0; */
  struct perlin_settings *s = &t->settings;
  /* x *= s->scale; */
  /* y *= s->scale; */
  double e =  perlin2d(x, y, s->freq, s->depth)
            + s->o1s * perlin2d(s->o1 * x, s->o1 * y, s->freq, s->depth)
            + s->o2s * perlin2d(s->o2 * x, s->o2 * y, s->freq, s->depth);
  return pow(e, s->exp) * s->amplitude;
}

typedef double (*noisefn)(void*,double,double);

void deriv(noisefn fn, void* data, double x, double y, double z1, double *dx, double *dy)
{
  static const double h = 0.01;
  double zx = fn(data, x + h, y);
  double zy = fn(data, x, y - h);
  *dx = (zx - z1)/h;
  *dy = (zy - z1)/h;
}

void reset_terrain(struct terrain *terrain, float size) {
    terrain->samples = NULL;
    terrain->n_samples = 0;
    terrain->fn = 0;
    terrain->size = size;

    struct entity *ent = get_entity(&terrain->entity_id);
    assert(ent);
}

void init_terrain(struct terrain *terrain, float size) {
    init_id(&terrain->entity_id);
    struct entity *ent  = new_entity(&terrain->entity_id); assert(ent);
    struct node *node   = get_node(&ent->node_id); assert(node);
    struct model *model = get_model(&terrain->model_id); assert(model);

    assert(node);
    assert(terrain->entity_id.index == 0);

    model->shading = SHADING_TERRAIN;
    ent->model_id = terrain->model_id;
    node_set_label(node, "terrain");
    ent->casts_shadows = 0;

    // this is scale for some reason!?
    node->pos[2] = 20.0;

    reset_terrain(terrain, size);
}

double offset_fn(struct terrain* terrain, double x, double y) {
    struct perlin_settings *perlin = &terrain->settings;
    double ox = perlin->ox;
    double oy = perlin->oy;
    return old_noisy_boi(terrain, ox+x, oy+y);
}

void gen_terrain_samples(struct terrain *terrain, float scale) {
    if (terrain->samples)
        free(terrain->samples);

    int n_samples =
        (terrain->size * terrain->size) / (scale * scale);

    /* struct point *samples = */
    /*   uniform_samples(n_samples, game->terrain.size); */

    static const double pdist = 24.0;

    struct point *samples =
        poisson_disk_samples(pdist, terrain->size, 30, &n_samples);

    /* remap_samples(samples, n_samples, game->terrain.size); */

    /* draw_samples(samples, pdist, n_samples, game->terrain.size); */

    terrain->samples = samples;
    terrain->n_samples = n_samples;
}

void create_terrain(struct terrain *terrain, float scale) {
    u32 i;
    const double size = terrain->size;

    float tmp1[3], tmp2[3];
    if (!terrain->n_samples)
        gen_terrain_samples(terrain, scale);
    assert(terrain->n_samples > 0);
    del_point2d_t *points = calloc(terrain->n_samples, sizeof(*points));

    float *verts = calloc(terrain->n_samples * 3, sizeof(*verts));
    /* float *normals = calloc(terrain->n_samples * 3, sizeof(*verts)); */

    terrain->fn = offset_fn;

    // 100 random samples from our noise function
    for (i = 0; i < (u32)terrain->n_samples; i++) {
        int n = i*3;
        double x, y;
        /* double dx, dy; */

        x = terrain->samples[i].x;
        y = terrain->samples[i].y;

        double z = terrain->fn(terrain, x, y);

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

    int num_verts = tri->num_triangles * 3;
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

        // centroid normals
        /* float c[3]; */
        /* c[0] = (v[0][0] + v[1][0] + v[2][0]) / 3.0; */
        /* c[1] = (v[0][1] + v[1][1] + v[2][1]) / 3.0; */
        /* c[2] = (v[0][2] + v[1][2] + v[2][2]) / 3.0; */
        /* double dx, dy; */
        /* deriv((noisefn)terrain->fn, terrain, c[0], c[1], c[2], &dx, &dy); */
        /* vec3_subtract(v[1], c, tmp1); */
        /* vec3_subtract(v[2], c, tmp2); */

        // standard normals
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

    struct entity *ent = get_entity(&terrain->entity_id);
    assert(ent);

    struct make_geometry mkgeom;
    init_make_geometry(&mkgeom);
    mkgeom.num_verts = num_verts;
    mkgeom.vertices = (float*)del_verts;
    mkgeom.normals = (float*)del_norms;
    mkgeom.indices = (u32*)del_indices;
    mkgeom.num_indices = num_verts;

    make_buffer_geometry(&mkgeom, &terrain->model.geom_id);

    delaunay2d_release(del);
    tri_delaunay2d_release(tri);

    free(points);
    free(verts);
    free(del_verts);
    free(del_norms);
    free(del_indices);
}


void destroy_terrain(struct terrain *terrain) {
    struct entity *ent = get_entity(&terrain->entity_id);
    assert(ent);
    destroy_buffer_geometry(&terrain->model.geom_id);
}
