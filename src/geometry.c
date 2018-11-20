
#include "geometry.h"
#include "util.h"
#include <assert.h>

static struct geometry_manager geom_manager;

void
destroy_buffer_geometry(struct geometry_id geom_id) {
    struct geometry *geom = get_geometry(geom_id);

    gpu_addr buffers[] = {
        geom->vertex.handle,
        geom->normal.handle,
        geom->color.handle,
        geom->index.handle,
        geom->tex_coord.handle
    };
    /* void glDeleteVertexArrays(GLsizei n, const GLuint *arrays); */
    /* glDisableVertexAttribArray(geom->buffer.vertex_buffer.handle); */
    /* check_gl(); */
    /* glDisableVertexAttribArray(geom->buffer.normal_buffer.handle); */
    /* check_gl(); */
    /* glDisableVertexAttribArray(geom->buffer.index_buffer.handle); */
    /* check_gl(); */
    check_gl();
    glDeleteBuffers(ARRAY_SIZE(buffers), buffers);
    check_gl();



    geom->has_vbos = 0;
}

void bind_geometry(struct geometry *geom, struct attributes *attrs) {
    bind_vbo(&geom->vertex, attrs->position);
    check_gl();
    if (geom->normal.handle && attrs->normal != 0xFFFFFFFF) {
        bind_vbo(&geom->normal, attrs->normal);
        check_gl();
    }
    if (geom->color.handle && attrs->color != 0xFFFFFFFF) {
        bind_vbo(&geom->color, attrs->color);
        check_gl();
    }
    if (geom->tex_coord.handle && attrs->tex_coord != 0xFFFFFFFF) {
        bind_vbo(&geom->tex_coord, attrs->tex_coord);
        check_gl();
    }
    bind_ibo(&geom->index);
    check_gl();
}

static void check_for_patches(struct gpu_program *program, int *type) {
    int has_tess_shader =
        get_program_shader(program, GL_TESS_EVALUATION_SHADER) != NULL;

    if (has_tess_shader) {
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        *type = GL_PATCHES;
    }
}


void render_geometry(struct geometry *geom, struct attributes *attrs,
                     struct gpu_program *program)
{
    int type = GL_TRIANGLES;
    //check_for_patches(program, &type);
    bind_geometry(geom, attrs);
    if (geom->num_indices) {
        glDrawElements(type,
                       geom->num_indices, /* count */
                       GL_UNSIGNED_INT,    /* type */
                       (void*)0            /* element array buffer offset */
                       );
        check_gl();
    }
    else {
        /* printf("nverts %d\n", geom->num_verts); */
        glDrawArrays(type, 0, geom->num_verts);
        check_gl();
    }
}

void init_make_geometry(struct make_geometry *mkgeom) {
    mkgeom->colors = NULL;
    mkgeom->normals = NULL;
    mkgeom->indices = NULL;
    mkgeom->vertices = NULL;
    mkgeom->tex_coords = NULL;
    mkgeom->num_uv_components = 2;
    mkgeom->num_verts = 0;
    mkgeom->num_indices = 0;
}

void init_geometry(struct geometry *geom) {
    geom->has_vbos = 0;
    init_vbo(&geom->vertex);
    init_vbo(&geom->color);
    init_vbo(&geom->normal);
    init_vbo(&geom->tex_coord);
    init_vbo(&geom->index);
}

void
make_buffer_geometry_(struct make_geometry *mkgeom, struct geometry *geom) {


    // VBOs
    geom->num_uv_components = mkgeom->num_uv_components;
    geom->num_verts = mkgeom->num_verts;
    geom->num_indices = mkgeom->num_indices;

    assert(mkgeom->vertices);
    /* assert(geom->normals); */
    /* assert(geom->indices); */
    /* assert(geom->num_indices >= 1); */

    /* printf("making vertex buffer\n"); */
    make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        mkgeom->vertices,
                        mkgeom->num_verts * 3 * (int)sizeof(*mkgeom->vertices),
                        &geom->vertex
                        );

    /* printf("making normal buffer\n"); */
    // cube normals
    if (mkgeom->normals != NULL)
        make_vertex_buffer(
                            GL_ARRAY_BUFFER,
                            mkgeom->normals,
                            mkgeom->num_verts * 3 * (int)sizeof(*mkgeom->normals),
                            &geom->normal
                            );

    // vertex colors
    if (mkgeom->colors != NULL)
        make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        mkgeom->colors,
                        mkgeom->num_verts * 3 * (int)sizeof(*mkgeom->colors),
                        &geom->color
                        );

    if (mkgeom->tex_coords != NULL) {
        assert(geom->num_uv_components);
        printf("%f %f %f %f\n",
               mkgeom->tex_coords[0],
               mkgeom->tex_coords[1],
               mkgeom->tex_coords[2],
               mkgeom->tex_coords[3]
               );

        make_uv_buffer(GL_ARRAY_BUFFER,
                       mkgeom->tex_coords,
                       mkgeom->num_verts * mkgeom->num_uv_components * (int)sizeof(*mkgeom->tex_coords),
                       &geom->tex_coord, geom->num_uv_components);
    }

    /* printf("making index buffer\n"); */
    // cube indices
    if (mkgeom->indices)
        make_index_buffer(
                        GL_ELEMENT_ARRAY_BUFFER,
                        mkgeom->indices,
                        mkgeom->num_indices * (int)sizeof(*mkgeom->indices),
                        &geom->index
                        );

    geom->has_vbos = 1;
}

struct geometry_id
make_buffer_geometry(struct make_geometry *mkgeom) {
    struct geometry_id geom_id;
    struct geometry *geom = new_geometry(&geom_id);

    make_buffer_geometry_(mkgeom, geom);
    return geom_id;
}



void geometry_centroid(struct geometry *geom, float *dest) {
    vec3_subtract(geom->max, geom->min, dest);
    vec3_scale(dest, 0.5, dest);
};

void init_geometry_manager() {
    geom_manager.num_geometry = 0;
}

struct geometry *get_geometry(struct geometry_id geom_id) {
    assert(geom_id.index != -1);
    assert(geom_id.index < geom_manager.num_geometry);
    return &geom_manager.geoms[geom_id.index];
}


struct geometry *new_geometry(struct geometry_id *geom_id) {
    u32 index = geom_manager.num_geometry++;
    struct geometry *g = &geom_manager.geoms[index];
    init_geometry(g);
    if (geom_id)
        geom_id->index = index;

    return g;
}

void init_geometry_id(struct geometry_id *id) {
    id->index = -1;
}

struct geometry *get_all_geometry(u32 *count) {
    *count = geom_manager.num_geometry;
    return &geom_manager.geoms[0];
}
