
#include "geometry.h"
#include <assert.h>

void
make_buffer_geometry(struct geometry *geom) {
  // VBOs

  assert(geom->vertices);
  assert(geom->normals);
  assert(geom->indices);
  assert(geom->num_indices >= 1);

  printf("making vertex buffer\n");
  make_vertex_buffer(
                     GL_ARRAY_BUFFER,
                     geom->vertices,
                     geom->num_verts * 3 * (int)sizeof(float),
                     &geom->buffer.vertex_buffer
                     );

  printf("making normal buffer\n");
  // cube normals
  make_vertex_buffer(
                     GL_ARRAY_BUFFER,
                     geom->normals,
                     geom->num_verts * 3 * (int)sizeof(float),
                     &geom->buffer.normal_buffer
                     );

  printf("making index buffer\n");
  // cube indices
  make_index_buffer(
                    GL_ELEMENT_ARRAY_BUFFER,
                    geom->indices,
                    geom->num_indices * (int)sizeof(u32),
                    &geom->buffer.index_buffer
                    );
}
