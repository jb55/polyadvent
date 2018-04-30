

#include "node.h"
#include "mat4.h"
#include "vec3.h"
#include <stdio.h>

struct node *node_init(struct node *node) {
  mat4_id(node->mat);
  vec3_all(node->pos, 0);
  vec3_all(node->rot, 0);
  vec3_all(node->scale, 1.0);
  node->children = 0;
  node->mirrored = 0;
  node->needs_recalc = 0;
  return node;
}

void node_translate(struct node *node, vec3 *p) {
  if (vec3_isall(p, 0))
    return;

  /* printf("translating %f %f %f\n", p[0], p[1], p[2]); */
  vec3_add(node->pos, p, node->pos);
  node->needs_recalc = 1;
}

int node_recalc(struct node *node) {
  float rotate_axis[3];

  if (!node->needs_recalc)
    return 0;

  mat4_id(node->mat);


  node->needs_recalc = 0;

  if (node->scale[0] != 1 || node->scale[1] != 1 || node->scale[2] != 1)
    mat4_scale(node->mat, node->scale, node->mat);

  //seomthing
  float m = node->mirrored ? -1 : 1;

  // FIXME: this seems bad?
  for (int i = 0; i < 3; ++i) {
    if (node->rot[i] != 0) {
      rotate_axis[0] = 0;
      rotate_axis[1] = 0;
      rotate_axis[2] = 0;
      rotate_axis[i] = 1;
      float x = node->pos[0];
      float y = node->pos[1];
      float z = node->pos[2];
      if (!node->mirrored)
        mat4_translate(node->mat, V3(x, y, z), node->mat);
      mat4_rotate(node->mat, node->rot[i], rotate_axis, node->mat);
      if (!node->mirrored)
        mat4_translate(node->mat, V3(-x, -y, -z), node->mat);
    }
  }

  if (node->pos[0] || node->pos[1] || node->pos[2])
    mat4_translate(node->mat, V3(node->pos[0]*m,node->pos[1]*m,node->pos[2]*m),
                   node->mat);


  /* if (node->pos[0] || node->pos[1] || node->pos[2]) */
  /*   mat4_translate(node->mat, node->pos, node->mat); */

  return 1;
}

