
#include "geometry.h"
#include "util.h"
#include <assert.h>

void
destroy_buffer_geometry(struct geometry *geom) {
    gpu_addr buffers[] = {
        geom->vbos.vertex.handle,
        geom->vbos.normal.handle,
        geom->vbos.color.handle,
        geom->vbos.index.handle,
        geom->vbos.tex_coord.handle
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
}

void bind_geometry(struct geometry *geom, struct attributes *attrs) {
    bind_vbo(&geom->vbos.vertex, attrs->position);
    check_gl();
    if (geom->vbos.normal.handle) {
        bind_vbo(&geom->vbos.normal, attrs->normal);
        check_gl();
    }
    if (geom->vbos.color.handle) {
        bind_vbo(&geom->vbos.color, attrs->color);
        check_gl();
    }
    if (geom->vbos.tex_coord.handle) {
        bind_vbo(&geom->vbos.tex_coord, attrs->tex_coord);
        check_gl();
    }
    bind_ibo(&geom->vbos.index);
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

    geom->vbos.color.handle = 0;
    geom->vbos.normal.handle = 0;
    geom->vbos.tex_coord.handle = 0;
}

void
make_buffer_geometry(struct geometry *geom) {
    // VBOs

    assert(geom->vertices);
    /* assert(geom->normals); */
    /* assert(geom->indices); */
    /* assert(geom->num_indices >= 1); */

    /* printf("making vertex buffer\n"); */
    make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        geom->vertices,
                        geom->num_verts * 3 * (int)sizeof(*geom->vertices),
                        &geom->vbos.vertex
                        );

    /* printf("making normal buffer\n"); */
    // cube normals
    if (geom->normals != NULL)
        make_vertex_buffer(
                            GL_ARRAY_BUFFER,
                            geom->normals,
                            geom->num_verts * 3 * (int)sizeof(*geom->normals),
                            &geom->vbos.normal
                            );

    // vertex colors
    if (geom->colors != NULL)
        make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        geom->colors,
                        geom->num_verts * 3 * (int)sizeof(*geom->colors),
                        &geom->vbos.color
                        );

    if (geom->tex_coords != NULL) {
        assert(geom->num_uv_components);
        printf("%f %f %f %f\n",
               geom->tex_coords[0],
               geom->tex_coords[1],
               geom->tex_coords[2],
               geom->tex_coords[3]
               );

        make_uv_buffer(GL_ARRAY_BUFFER,
                       geom->tex_coords,
                       geom->num_verts * geom->num_uv_components * (int)sizeof(*geom->tex_coords),
                       &geom->vbos.tex_coord, geom->num_uv_components);
    }

    /* printf("making index buffer\n"); */
    // cube indices
    if (geom->indices)
        make_index_buffer(
                        GL_ELEMENT_ARRAY_BUFFER,
                        geom->indices,
                        geom->num_indices * (int)sizeof(*geom->indices),
                        &geom->vbos.index
                        );
}



void geometry_centroid(struct geometry *geom, float *dest) {
    vec3_subtract(geom->max, geom->min, dest);
    vec3_scale(dest, 0.5, dest);
};
