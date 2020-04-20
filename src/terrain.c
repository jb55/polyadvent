
#include "terrain.h"
#include "util.h"
#include "delaunay.h"
#include "debug.h"
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
  double e =  perlin2d(t->settings.seed, x, y, s->freq, s->depth)
      + s->o1s * perlin2d(t->settings.seed, s->o1 * x, s->o1 * y, s->freq, s->depth)
      + s->o2s * perlin2d(t->settings.seed, s->o2 * x, s->o2 * y, s->freq, s->depth);
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

    struct entity *ent  = &static_entities()[entity_terrain];
    struct node *node   = &static_nodes()[node_terrain];
    struct model *model = new_model(&ent->model_id); assert(model);
    /* struct model *model = init_model(&ent->model_id); assert(model); */

    terrain->entity_id = make_static_id(entity_terrain);

    assert(terrain->entity_id.index == 0);

    model->shading = SHADING_TERRAIN;
    node_set_label(node, "terrain");
    ent->flags &= ~ENT_CASTS_SHADOWS;

    // this is scale for some reason!?
    /* node->pos[2] = 20.0; */

    reset_terrain(terrain, size);
}

double offset_fn(struct terrain* terrain, double x, double y) {
    struct perlin_settings *perlin = &terrain->settings;
    double ox = perlin->ox;
    double oy = perlin->oy;
    return old_noisy_boi(terrain, ox+x, oy+y);
}

void gen_terrain_samples(struct terrain *terrain, float scale, const double pdist) {

    debug("generating terrain samples\n");
    if (terrain->samples)
        free(terrain->samples);

    int n_samples =
        (terrain->size * terrain->size) / (scale * scale);

    /* struct point *samples = */
    /*   uniform_samples(n_samples, game->terrain.size); */

    struct point *samples =
        poisson_disk_samples(pdist, terrain->size, 30, &n_samples);

    /* remap_samples(samples, n_samples, game->terrain.size); */

    /* draw_samples(samples, pdist, n_samples, game->terrain.size); */

    terrain->samples = samples;
    terrain->n_samples = n_samples;

}

static inline struct terrain_cell *index_terrain_cell(struct terrain *terrain, int x, int y)
{
    if (x < 0 || y < 0 || x >= terrain->n_cells || y >= terrain->n_cells) {
        assert(!"terrain oob");
        return NULL;
    }

    return &terrain->grid[y * terrain->n_cells + x];
}

static inline struct terrain_cell *query_terrain_cell(struct terrain *terrain,
                                                      float x, float y,
                                                      int *grid_x, int *grid_y)
{ 
    assert(x >= 0);
    assert(y >= 0);
    assert(x < terrain->size);
    assert(y < terrain->size);
    *grid_x = grid_index(terrain, x);
    *grid_y = grid_index(terrain, y);

    return index_terrain_cell(terrain, *grid_x, *grid_y);
}

void query_terrain_grid(struct terrain *terrain, float x, float y,
                        struct terrain_cell *cells[9])
{
    int grid_x, grid_y;

    // middle
    cells[4] = query_terrain_cell(terrain, x, y, &grid_x, &grid_y);

    // top row
    cells[0] = index_terrain_cell(terrain, grid_x - 1, grid_y - 1);
    cells[1] = index_terrain_cell(terrain, grid_x,     grid_y - 1);
    cells[2] = index_terrain_cell(terrain, grid_x + 1, grid_y - 1);

    // left, right
    cells[3] = index_terrain_cell(terrain, grid_x - 1, grid_y);
    cells[5] = index_terrain_cell(terrain, grid_x + 1, grid_y);

    // bottom row
    cells[6] = index_terrain_cell(terrain, grid_x - 1, grid_y + 1);
    cells[7] = index_terrain_cell(terrain, grid_x    , grid_y + 1);
    cells[8] = index_terrain_cell(terrain, grid_x + 1, grid_y + 1);
}

static void insert_grid_vertex(struct terrain_cell *cell, int ind)
{
    assert(cell->vert_count + 1 <= MAX_CELL_VERTS);
    cell->verts_index[cell->vert_count++] = ind;
}

#ifdef DEBUG
static int terrain_cell_debug(struct terrain *terrain, struct terrain_cell *cell, int ind, vec3 *pos)
{
    int ok = 0;
    entity_id *ent_id = &cell->debug_ent[ind];

    if (is_null_id(ent_id)) {
        assert(cell->verts_index[ind] < terrain->n_verts);
        float *vert = &terrain->verts[cell->verts_index[ind]];
        int gx = grid_index(terrain, pos[0]);
        int gy = grid_index(terrain, pos[1]);
        int vgx = grid_index(terrain, vert[0]);
        int vgy = grid_index(terrain, vert[1]);
        debug("creating new grid_debug entity at %f %f %f, cell (%d, %d) vert_cell(%d, %d)\n",
                vert[0], vert[1], vert[2],
                gx, gy, vgx, vgy);
        new_debug_entity(ent_id, vert);
        ok |= 1;
    }

    return ok;
}
#endif /* DEBUG */


#ifdef DEBUG
static int collide_terrain_debug(struct terrain *terrain, struct terrain_cell *cell, int ind, vec3 *pos)
{
    int ok = 0;
    for (int j = 0; j < cell->vert_count; j++) {
        ok |= terrain_cell_debug(terrain, cell, ind, pos);
    }

    return ok;
}
#endif /* DEBUG */


void create_terrain(struct terrain *terrain, float scale, int seed) {
    u32 i;
    const double size = terrain->size;

    static const double pdist = 24.0;
    terrain->settings.seed = seed;

    float tmp1[3], tmp2[3];
    if (!terrain->n_samples) {
        gen_terrain_samples(terrain, scale, pdist);
        /* save_samples(terrain->samples, seed, terrain->n_samples); */
    }
    assert(terrain->n_samples > 0);

    del_point2d_t *points = calloc(terrain->n_samples, sizeof(*points));
    float *verts = calloc(terrain->n_samples * 3, sizeof(*verts));
    terrain->cell_size = pdist;
    terrain->n_cells = round(size / terrain->cell_size) + 1;
    debug("n_cells %d\n", terrain->n_cells);

    struct terrain_cell *grid =
        calloc(terrain->n_cells * terrain->n_cells, sizeof(struct terrain_cell));

    terrain->grid = grid;

    /* float *normals = calloc(terrain->n_samples * 3, sizeof(*verts)); */

    terrain->fn = offset_fn;

    // n random samples from our noise function
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

        int grid_x = verts[n] / terrain->cell_size;
        int grid_y = verts[n+1] / terrain->cell_size;

        struct terrain_cell *cell =
            index_terrain_cell(terrain, grid_x, grid_y);

        assert(cell);

        insert_grid_vertex(cell, n);

        static const double limit = 1.4;
        if (x < limit || x > size-limit || y < limit || y > size-limit)
        verts[n+2] = 0;
        else
        verts[n+2] = (float)z;
    }

    delaunay2d_t *del = delaunay2d_from(points, terrain->n_samples);
    tri_delaunay2d_t *tri = tri_delaunay2d_from(del);

    int num_verts = tri->num_triangles * 3;
    float *del_verts = malloc(num_verts * 3 * sizeof(*del_verts));
    float *del_norms = malloc(num_verts * 3 * sizeof(*del_norms));
    u32   *del_indices = malloc(num_verts * sizeof(*del_indices));
    struct vert_tris *vert_tris = calloc(num_verts, sizeof(struct vert_tris));
    terrain->vtris = vert_tris;

    /// XXX (perf): we should be able to do this directly from del instead of
    /// triangulating with tri
    debug("terrain n_tris %d\n", tri->num_triangles);
    for (i = 0; i < tri->num_triangles; ++i) {
        int nv = i * 3;
        int ndv = i * 9;

        int p[3] = {
            tri->tris[nv + 0]*3,
            tri->tris[nv + 1]*3,
            tri->tris[nv + 2]*3,
        };

        float *v[3] = {
            &verts[p[0]],
            &verts[p[1]],
            &verts[p[2]]
        };

        for (int j = 0; j < 3; j++) {
            struct vert_tris *vtris = &vert_tris[p[j]/3];
            assert(vtris->tri_count + 1 < MAX_VERT_TRIS);
            struct tri *t = &vtris->tris[vtris->tri_count++];
            assert(sizeof(p) == sizeof(t->vert_indices));
            memcpy(t->vert_indices, p, sizeof(p));

            int ind = ndv + j*3;
            del_verts[ind+0] = v[j][0];
            del_verts[ind+1] = v[j][1];
            del_verts[ind+2] = v[j][2];
        }

        // normals
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

    struct model *model = get_model(&ent->model_id); assert(model);
    struct geometry *geom = get_geometry(&model->geom_id); assert(geom);

    assert(mkgeom.joint_weights == 0);
    make_buffer_geometry(&mkgeom, geom);

    delaunay2d_release(del);
    tri_delaunay2d_release(tri);

    assert(del_verts);

    free(points);
    // needed for collision
    /* free(verts); */
    free(del_verts);

    terrain->verts = verts;
    terrain->n_verts = num_verts;

    // we might need norms in memory eventually as well ?
    free(del_norms);
    free(del_indices);

}


void destroy_terrain(struct terrain *terrain) {
    struct entity *ent = get_entity(&terrain->entity_id); assert(ent);
    struct model *model = get_model(&ent->model_id); assert(model);
    destroy_buffer_geometry(&model->geom_id);
}


void update_terrain(struct terrain *terrain, const double pdist) {
    static int first = 1;
    static float last_scale = -1.0;

    struct entity *ent = get_entity(&terrain->entity_id);
    assert(ent);
    struct perlin_settings *ts = &terrain->settings;
    struct node *tnode = get_node(&ent->node_id);
    assert(tnode);

    debug("updating terrain\n");

    if (first) {
        reset_terrain(terrain, terrain->size);
        /* tnode->pos[0] = rand_0to1() * terrain->size; */
        /* tnode->pos[1] = rand_0to1() * terrain->size; */
        first = 0;
    }

    ts->ox = tnode->pos[0];
    ts->oy = tnode->pos[1];

    double scale = tnode->pos[2] * 0.0015;
    if (scale == 0) scale = 1.0;

    printf("terrain %f %f %f\n", tnode->pos[0], tnode->pos[1], tnode->pos[2]);

    /* ts.o1s = fabs(sin(1/n) * 0.25); */
    /* ts.o1 = fabs(cos(n*0.2) * 0.5); */
    /* ts.o2s = fabs(cos(n+2) * 0.5); */
    /* ts.o2 = fabs(sin(n*0.02) * 2); */
    ts->freq = scale * 0.08;
    ts->amplitude = 70.0;

    /* if (terrain->fn) */
    /*     destroy_terrain(terrain); */

    /* const double pdist = min(5.0, max(1.1, 1.0/scale*1.4)); */

    /* printf("pdist %f\n", pdist); */

    if (last_scale == -1.0 || fabs(scale - last_scale) > 0.00001) {
        gen_terrain_samples(terrain, scale, pdist);
    }

    last_scale = scale;
    create_terrain(terrain, scale, 0);
}
