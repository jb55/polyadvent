

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "entity.h"
#include "model.h"
#include "debug.h"

#define MAX_CELL_VERTS 4
#define MAX_VERT_TRIS 24

struct point;

struct perlin_settings {
    double freq;
    int depth;
    int seed;
    double amplitude;
    double ox, oy;
    double o1, o1s;
    double o2, o2s;
    double scale;
    double exp;
};

struct tri {
#ifdef DEBUG
    entity_id debug_id;
#endif
    int vert_indices[3];
};

struct vert_tris {
    u8 tri_count;
    struct tri tris[MAX_VERT_TRIS];
};

struct terrain_cell {
    u8 vert_count;
    int verts_index[MAX_CELL_VERTS];
#ifdef DEBUG
    entity_id debug_ent[MAX_CELL_VERTS];
#endif
};

struct terrain {
    entity_id entity_id;
    struct terrain_cell *grid;
    struct vert_tris *vtris;
    int n_cells; // all cells = grid_cells^2
    float cell_size;
    float *verts;
    int n_verts;
    struct perlin_settings settings;
    struct point *samples;
    double (*fn)(struct terrain *, double, double);
    int n_samples;
    float pdist;
    double size;
};

double old_noisy_boi(struct terrain *, double x, double y);


void collide_terrain(struct terrain *terrain, struct node *node, struct model *model, vec3 *move);
void update_terrain(struct terrain *terrain, const double pdist);
void gen_terrain_samples(struct terrain *terrain, float scale, const double pdist);
void init_terrain(struct terrain *terrain, float size);
void reset_terrain(struct terrain *terrain, float size);
void create_terrain(struct terrain *terrain, float scale, int seed);
void destroy_terrain(struct terrain *terrain);
void query_terrain_grid(struct terrain *terrain, float x, float y, struct terrain_cell *cells[9]);

static inline int grid_index(struct terrain *terrain, float x) {
    return x / terrain->cell_size;
}

static inline void grid_pos_debug(const char *thing, struct terrain *terrain, float *pos) {
    int gx = grid_index(terrain, pos[0]);
    int gy = grid_index(terrain, pos[1]);
    debug("%s grid pos (%d, %d)\n", thing, gx, gy);
}

#endif /* POLYADVENT_TERRAIN_H */
