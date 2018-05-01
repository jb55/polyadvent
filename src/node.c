

#include "node.h"
#include "mat4.h"
#include "vec3.h"
#include <stdio.h>
#include <assert.h>

struct node *node_init(struct node *node) {
  mat4_id(node->mat);
  vec3_all(node->pos, 0);
  vec3_all(node->rot, 0);
  vec3_all(node->scale, 1.0);
  node->n_children = 0;
  for (int i = 0; i < MAX_NODE_CHILDREN; ++i)
    node->children[i] = NULL;
  node->parent = NULL;
  node->mirrored = 0;
  node->label = "unknown";
  node->needs_recalc = 0;
  node_mark_for_recalc(node);

  return node;
}

void node_translate(struct node *node, vec3 *p) {
  if (vec3_isall(p, 0))
    return;

  /* printf("translating %f %f %f\n", p[0], p[1], p[2]); */
  vec3_add(node->pos, p, node->pos);
  node_mark_for_recalc(node);
}

void node_rotate(struct node *node, vec3 *p) {
  if (vec3_isall(p, 0))
    return;

  /* printf("translating %f %f %f\n", p[0], p[1], p[2]); */
  vec3_add(node->rot, p, node->rot);
  node_mark_for_recalc(node);
}

int node_needs_recalc(struct node *node) {
  return (node->parent && node->parent->needs_recalc) || node->needs_recalc;
}

void node_mark_for_recalc(struct node *node) {
  static int j = 0;

  if (node->needs_recalc)
    return;

  node->needs_recalc = 1;

  for (int i = 0; i < node->n_children; ++i) {
    printf("%d %s marking child %s for recalc\n", j++, node->label, node->children[i]->label);
    node_mark_for_recalc(node->children[i]);
  }
}

static void node_recalc_children(struct node *node) {
  for (int i = 0; i < node->n_children; ++i) {
    node_recalc(node->children[i]);
  }
}

int node_recalc(struct node *node) {
  float rotate_axis[3];
  float tmp[16];

  assert(node);

  if (!node_needs_recalc(node)) {
    // XXX: do I need this?
    goto recalc_children;
  }

  printf("recalculating %s\n", node->label);

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
      mat4_rotate(node->mat, node->rot[i]*m, rotate_axis, node->mat);
      if (!node->mirrored)
        mat4_translate(node->mat, V3(-x, -y, -z), node->mat);
    }
  }

  if (node->pos[0] || node->pos[1] || node->pos[2])
    mat4_translate(node->mat, V3(node->pos[0]*m,node->pos[1]*m,node->pos[2]*m),
                   node->mat);

  /* if (node->pos[0] || node->pos[1] || node->pos[2]) */
  /*   mat4_translate(node->mat, node->pos, node->mat); */

  if (node->parent) {
    assert(!node->parent->needs_recalc);
    mat4_multiply(node->mat, node->parent->mat, node->mat);
  }

recalc_children:
  node_recalc_children(node);

  return 1;
}


void node_attach(struct node *node, struct node *to) {
  assert(to->n_children <= MAX_NODE_CHILDREN);

  node->parent = to;
  to->children[to->n_children++] = node;
}
