
#include "geometry.h"
#include "util.h"
#include <assert.h>

void
destroy_buffer_geometry(struct geometry *geom) {
    gpu_addr buffers[] = {
        geom->buffer.vertex_buffer.handle,
        geom->buffer.normal_buffer.handle,
        geom->buffer.color_buffer.handle,
        geom->buffer.index_buffer.handle
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


void init_geometry(struct geometry *geom) {
    geom->colors = NULL;
}

void
make_buffer_geometry(struct geometry *geom) {
    // VBOs

    assert(geom->vertices);
    assert(geom->normals);
    assert(geom->indices);
    assert(geom->num_indices >= 1);

    /* printf("making vertex buffer\n"); */
    make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        geom->vertices,
                        geom->num_verts * 3 * (int)sizeof(*geom->vertices),
                        &geom->buffer.vertex_buffer
                        );

    /* printf("making normal buffer\n"); */
    // cube normals
    make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        geom->normals,
                        geom->num_verts * 3 * (int)sizeof(*geom->normals),
                        &geom->buffer.normal_buffer
                        );

    // vertex colors
    if (geom->colors != NULL)
        make_vertex_buffer(
                        GL_ARRAY_BUFFER,
                        geom->colors,
                        geom->num_verts * 3 * (int)sizeof(*geom->colors),
                        &geom->buffer.color_buffer
                        );

    /* printf("making index buffer\n"); */
    // cube indices
    make_index_buffer(
                    GL_ELEMENT_ARRAY_BUFFER,
                    geom->indices,
                    geom->num_indices * (int)sizeof(*geom->indices),
                    &geom->buffer.index_buffer
                    );
}
