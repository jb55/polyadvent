
#include "geometry.h"
#include "util.h"
#include "resource.h"
#include "debug.h"
#include "static_resources.h"
#include <assert.h>

struct resource_manager geom_manager;

void
destroy_buffer_geometry(geometry_id *geom_id) {
    struct geometry *geom = get_geometry(geom_id);
    gpu_addr buffers[n_vertex_attrs];

    for (int i = 0; i < n_vertex_attrs; i++)
        buffers[i] = geom->vbos[i].handle;
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

void bind_geometry(struct geometry *geom, gpu_addr *vertex_attrs) {
    struct vbo *vbo;
    for (int i = 0; i < n_vertex_attrs; i++) {
        vbo = &geom->vbos[i];
        if (vbo->handle && vertex_attrs[i] != 0xFFFFFFFF) {
            bind_vbo(vbo, vertex_attrs[i], vbo->component_type);
            check_gl();
        }
    }
}

static void check_for_patches(struct gpu_program *program, int *type) {
    int has_tess_shader =
        get_program_shader(program, GL_TESS_EVALUATION_SHADER) != NULL;

    if (has_tess_shader) {
        glPatchParameteri(GL_PATCH_VERTICES, 3);
        *type = GL_PATCHES;
    }
}


void render_geometry(struct geometry *geom, gpu_addr *vertex_attrs,
                     struct gpu_program *program)
{
    int type = GL_TRIANGLES;
    //check_for_patches(program, &type);
    bind_geometry(geom, vertex_attrs);
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
    *mkgeom = (struct make_geometry){
      .colors = NULL,
      .normals = NULL,
      .indices = NULL,
      .vertices = NULL,
      .tex_coords = NULL,
      .joint_ids = NULL,
      .joint_weights = NULL,
      .num_uv_components = 2,
      .num_verts = 0,
      .num_indices = 0,
    };
}

void init_geometry(struct geometry *geom) {
    geom->has_vbos = 0;

    for (int i = 0; i < n_vertex_attrs; i++)
        init_vbo(&geom->vbos[i]);

    geom->num_uv_components = 2;
}

void make_buffer_geometry(struct make_geometry *mkgeom, struct geometry *geom)
{
    init_geometry(geom);

    // VBOs
    geom->num_uv_components = mkgeom->num_uv_components;
    geom->num_verts = mkgeom->num_verts;
    geom->num_indices = mkgeom->num_indices;

    assert(mkgeom->num_verts);
    assert(mkgeom->vertices);
    /* assert(geom->normals); */
    /* assert(geom->indices); */
    /* assert(geom->num_indices >= 1); */

    /* printf("making vertex buffer\n"); */
    make_float_vertex_buffer(&geom->vbos[va_position],
                             mkgeom->vertices,
                             mk_num_elements(mkgeom->num_verts),
                             mk_components(3)
                             );

    /* printf("making normal buffer\n"); */
    // cube normals
    if (mkgeom->normals) {
        make_float_vertex_buffer(&geom->vbos[va_normal],
                                 mkgeom->normals,
                                 mk_num_elements(mkgeom->num_verts),
                                 mk_components(3)
                                 );
    }
    if (mkgeom->joint_ids) {
        make_int_vertex_buffer(&geom->vbos[va_joint_ids],
                               mkgeom->joint_ids,
                               mk_num_elements(mkgeom->num_verts),
                               mk_components(3)
                               );
    }

    // vertex colors
    if (mkgeom->colors) {
        make_float_vertex_buffer(&geom->vbos[va_color],
                                 mkgeom->colors,
                                 mk_num_elements(mkgeom->num_verts),
                                 mk_components(3)
                                 );
    }

    if (mkgeom->tex_coords != NULL) {
        assert(geom->num_uv_components);

        make_uv_buffer(&geom->vbos[va_tex_coord],
                       mkgeom->tex_coords,
                       mk_num_elements(geom->num_verts),
                       mk_components(geom->num_uv_components)
                       );
    }

    /* printf("making index buffer\n"); */
    // cube indices
    if (mkgeom->indices)
        make_index_buffer(&geom->vbos[va_index],
                          mkgeom->indices,
                          mk_num_elements(mkgeom->num_indices)
                          );

    geom->has_vbos = 1;
}


void geometry_centroid(struct geometry *geom, float *dest) {
    vec3_subtract(geom->max, geom->min, dest);
    vec3_scale(dest, 0.5, dest);
};


void init_geometry_manager() {
    init_resource_manager(&geom_manager, sizeof(struct geometry),
                          DEF_NUM_GEOMETRY, MAX_GEOMETRY, "geometry",
                          NUM_STATIC_MODELS);

    for (int i = 0; i < NUM_STATIC_MODELS; i++) {
        init_geometry(&static_geometry()[i]);
    }
}

struct geometry *get_geometry(geometry_id *geom_id) {
    return get_resource(&geom_manager, geom_id);
}


struct geometry *new_geometry(geometry_id *geom_id) {
    struct geometry *geom = new_resource(&geom_manager, geom_id);
    /* debug("new geometry %llu\n", geom_id->uuid); */
    return geom;
}

struct geometry *get_all_geometry(u32 *count, geometry_id **ids) {
    return get_all_resources(&geom_manager, count, ids);
}

void destroy_geometry(geometry_id *geom_id)
{
    struct geometry *geom = get_geometry(geom_id); assert(geom);
    struct vbo *vbo;

    for (int i = 0; i < n_vertex_attrs; i++) {
        vbo = &geom->vbos[i];
        if (vbo->handle)
            glDeleteBuffers(1, &vbo->handle);
    }

    destroy_resource(&geom_manager, geom_id);
}
