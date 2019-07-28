
#include "terrain_collision.h"


struct grid_query {
    struct terrain_cell *cell;
    int cell_vert_index;
    float distance;
};

struct tri_query {
    struct vert_tri *vtri;
};

static void get_closest_verts(struct terrain *terrain,
                              vec3 *pos,
                              struct grid_query closest[3],
                              struct terrain_cell *cells[9]
                              )
{
    for (int i = 0; i < 9; i++) {
        struct terrain_cell *cell = cells[i];
        for (int j = 0; j < cell->vert_count; j++) {
            vec3 *vpos = &terrain->verts[cell->verts_index[j]];
            float d = vec3_distsq(pos, vpos);

            if (d < closest[0].distance) {
                closest[2] = closest[1];
                closest[1] = closest[0];
                closest[0] = (struct grid_query) {
                  .distance = d,
                  .cell = cell,
                  .cell_vert_index = j
                };
            }
            else if (d < closest[1].distance) {
                closest[2] = closest[1];
                closest[1] = (struct grid_query) {
                   .distance = d,
                   .cell = cell,
                   .cell_vert_index = j
                };
            }
            else if (d < closest[2].distance) {
                closest[2].distance = d;
                closest[2].cell_vert_index = j;
                closest[2].cell = cell;
            }
        }
    }
}

static inline float sign (float *p1, float *p2, float *p3)
{
    return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1]);
}

bool point_in_tri(float *pt, float *v1, float *v2, float *v3)
{
    float d1, d2, d3;
    bool has_neg, has_pos;

    d1 = sign(pt, v1, v2);
    d2 = sign(pt, v2, v3);
    d3 = sign(pt, v3, v1);

    has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
    has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

    return !(has_neg && has_pos);
}

static void get_closest_tris(struct terrain *terrain, struct vert_tris *vtris)
{

}

static void terrain_tri_debug(float *verts, struct tri *tri)
{

    if (is_null_id(&tri->debug_id)) {

        float tmp[3];

        float *v1 = &verts[tri->vert_indices[0]];
        float *v2 = &verts[tri->vert_indices[1]];
        float *v3 = &verts[tri->vert_indices[2]];

        /* debug("v1 %f,%f v2 %f,%f v3 %f,%f\n", */
        /*       v1[0], v1[1], */
        /*       v2[0], v2[1], */
        /*       v3[0], v3[1]); */

        vec3_subtract(v1, v2, tmp);

        /* debug("tmp %f %f\n", tmp[0], tmp[1]); */

        vec3_scale(tmp, 0.5, tmp);
        vec3_subtract(v1, tmp, tmp);
        vec3_subtract(v3, tmp, tmp);
        vec3_scale(tmp, 0.5, tmp);
        vec3_subtract(v3, tmp, tmp);

        /* debug("creating new grid_debug entity at %f %f %f\n", tmp[0], tmp[1], tmp[2]); */

        new_debug_entity(&tri->debug_id, tmp);
    }

}

void collide_terrain(struct terrain *terrain, struct node *node, struct model *model, vec3 *move)
{
    struct terrain_cell *cells[9] = {0};
    struct grid_query queries[3];
    for (int i = 0; i < 3; i++) {
        queries[i].distance = FLT_MAX;
        queries[i].cell = NULL;
        queries[i].cell_vert_index = -1;
    }

    float *pos = node_world(node);

    query_terrain_grid(terrain, pos[0], pos[1], cells);

    get_closest_verts(terrain, pos, queries, cells);

    int closest_vind = queries[0].cell->verts_index[queries[0].cell_vert_index];
    struct vert_tris *vtris = &terrain->vtris[closest_vind / 3];

    for (int i = 0; i < vtris->tri_count; i++) {;
        /* terrain_cell_debug(terrain, queries[i].cell, queries[i].cell_vert_index, pos); */
        terrain_tri_debug(terrain->verts, &vtris->tris[i]);
    }
}


    /* assert(closest_cells[0]); */
    /* assert(closest_cells[1]); */
    /* assert(closest_cells[2]); */
