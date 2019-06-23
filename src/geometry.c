
#include "geometry.h"
#include "util.h"
#include <assert.h>

void
destroy_buffer_geometry(struct geometry *geom) {
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
}

void bind_geometry(struct geometry *geom, gpu_addr *vertex_attrs) {
    struct vbo *vbo;
    for (int i = 0; i < n_vertex_attrs; i++) {
        vbo = &geom->vbos[i];
        if (vbo->handle && vertex_attrs[i] != 0xFFFFFFFF)
            bind_vbo(vbo, vertex_attrs[i], vbo->component_type);
        check_gl();
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
    if (geom->indices) {
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


void init_geometry(struct geometry *geom) {
    geom->colors = NULL;
    geom->normals = NULL;
    geom->indices = NULL;
    geom->vertices = NULL;
    geom->tex_coords = NULL;
    geom->num_uv_components = 2;

    geom->vbos[va_color].handle = 0;
    geom->vbos[va_normal].handle = 0;
    geom->vbos[va_tex_coord].handle = 0;
}

void
make_buffer_geometry(struct geometry *geom) {
    // VBOs

    assert(geom->vertices);

    /* assert(geom->normals); */
    /* assert(geom->indices); */
    /* assert(geom->num_indices >= 1); */

    /* printf("making vertex buffer\n"); */
    make_float_vertex_buffer(&geom->vbos[va_position],
                             geom->vertices,
                             mk_num_elements(geom->num_verts),
                             mk_components(3)
                             );

    /* printf("making normal buffer\n"); */
    // cube normals
    if (geom->normals)
        make_float_vertex_buffer(&geom->vbos[va_normal],
                                 geom->normals,
                                 mk_num_elements(geom->num_verts),
                                 mk_components(3)
                                 );

    if (geom->joint_ids) {
        make_int_vertex_buffer(&geom->vbos[va_joint_ids],
                               geom->joint_ids,
                               mk_num_elements(geom->num_verts),
                               mk_components(3)
                               );
    }

    // vertex colors
    if (geom->colors) {
        make_float_vertex_buffer(&geom->vbos[va_color],
                                 geom->colors,
                                 mk_num_elements(geom->num_verts),
                                 mk_components(3)
                                 );
    }

    if (geom->tex_coords != NULL) {
        assert(geom->num_uv_components);

        make_uv_buffer(&geom->vbos[va_tex_coord],
                       geom->tex_coords,
                       mk_num_elements(geom->num_verts),
                       mk_components(geom->num_uv_components)
                       );
    }

    /* printf("making index buffer\n"); */
    // cube indices
    if (geom->indices)
        make_index_buffer(&geom->vbos[va_index],
                          geom->indices,
                          mk_num_elements(geom->num_indices)
                         );
}



void geometry_centroid(struct geometry *geom, float *dest) {
    vec3_subtract(geom->max, geom->min, dest);
    vec3_scale(dest, 0.5, dest);
};
