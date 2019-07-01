
#include "quickhull.h"
#include "geometry.h"
#include "util.h"


static void qh_mesh_to_geom(qh_mesh_t *qh, struct make_geometry *geom) {
    assert(!geom->vertices);
    assert(!geom->indices);
    float *new_normals = malloc(sizeof(float) * 3 * qh->nvertices);

    geom->vertices = (float*)qh->vertices;
    geom->normals = (float*)qh->normals;
    geom->indices = qh->indices;
    geom->num_verts = qh->nvertices;
    geom->num_indices = qh->nindices;

    for (u32 i = 0; i < qh->nnormals; i++) {
        int ndv = i * 9;

        qh_vertex_t *n = &qh->normals[i];
        for (int j = 0; j < 9; j++) {
            new_normals[ndv+j] = n->v[j%3];
        }
    }

    geom->normals = new_normals;
}


void proc_sphere(struct geometry *geom) {
    struct make_geometry mkgeom;
    init_make_geometry(&mkgeom);

    const int n = 50;
    qh_vertex_t *vertices = malloc(n*sizeof(qh_vertex_t));
    const float radius = 2.0;


    for (int i = 0; i < n; ++i) {
        float a0 = (rand_0to1() * TAU);
        float a1 = (rand_0to1() * TAU);
        vertices[i].z = sin(a0) * radius;
        vertices[i].x = cos(a1) * cos(a0) * rand_0to1() * radius;
        vertices[i].y = sin(a1) * cos(a0) * rand_0to1() * radius;
    }

    qh_mesh_t mesh = qh_quickhull3d(vertices, n);
    qh_mesh_to_geom(&mesh, &mkgeom);
    make_buffer_geometry(&mkgeom, geom);

    free(mkgeom.normals);
    qh_free_mesh(mesh);
}

