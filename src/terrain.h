

#ifndef POLYADVENT_TERRAIN_H
#define POLYADVENT_TERRAIN_H

#include "entity.h"
#include "model.h"

#define MAX_CELL_VERTS 4

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

struct terrain_cell {
    u8 vert_count;
    int verts_index[MAX_CELL_VERTS];
    entity_id debug_ent[MAX_CELL_VERTS];
};

struct terrain {
    entity_id entity_id;
    struct terrain_cell *grid;
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

void update_terrain(struct terrain *terrain, const double pdist);
void gen_terrain_samples(struct terrain *terrain, float scale, const double pdist);
void init_terrain(struct terrain *terrain, float size);
void reset_terrain(struct terrain *terrain, float size);
void create_terrain(struct terrain *terrain, float scale, int seed);
void destroy_terrain(struct terrain *terrain);
void query_terrain_grid(struct terrain *terrain, float x, float y, struct terrain_cell *cells[9]);

#endif /* POLYADVENT_TERRAIN_H */
