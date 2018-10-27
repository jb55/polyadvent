
#include "geometry.h"
#include "util.h"
#include <assert.h>

void
destroy_buffer_geometry(struct geometry *geom) {
    gpu_addr buffers[] = {
        geom->vbos.vertex.handle,
        geom->vbos.normal.handle,
        geom->vbos.color.handle,
        geom->vbos.index.handle
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

void render_geometry(struct geometry *geom, struct attributes *attrs)
{
    bind_vbo(&geom->vbos.vertex, attrs->position);
    bind_vbo(&geom->vbos.normal, attrs->normal);
    if (geom->vbos.color.handle)
        bind_vbo(&geom->vbos.color, attrs->color);
    bind_ibo(&geom->vbos.index);

    glDrawElements(GL_TRIANGLES,
                   geom->num_indices, /* count */
                   GL_UNSIGNED_INT,    /* type */
                   (void*)0            /* element array buffer offset */
                   );
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
                        &geom->vbos.vertex
                        );

    /* printf("making normal buffer\n"); */
    // cube normals
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

    /* printf("making index buffer\n"); */
    // cube indices
    make_index_buffer(
                    GL_ELEMENT_ARRAY_BUFFER,
                    geom->indices,
                    geom->num_indices * (int)sizeof(*geom->indices),
                    &geom->vbos.index
                    );
}
