

#include "node.h"
#include "mat_util.h"
#include <stdio.h>
#include <assert.h>

struct node *node_init(struct node *node) {
  mat4_id(node->mat);
  mat4_id(node->local);
  vec3_all(node->pos, 0);
  vec3_all(node->rot, 0);
  vec3_all(node->scale, 1.0);
  quat_id(node->orientation);
  node->n_children = 0;
  for (int i = 0; i < MAX_NODE_CHILDREN; ++i)
    node->children[i] = NULL;
  node->parent = NULL;
  node->label = "unknown";
  node->needs_recalc = 0;
  node->custom_update = 0;
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

void node_rotate(struct node *node, vec3 *axis_angles) {
  if (vec3_isall(axis_angles, 0))
    return;

  for (int i = 0; i < 3; ++i) {
    float axis[3] = {0};
    float quat[4];
    axis[i] = 1;
    quat_axis_angle(axis, axis_angles[i], quat);
    quat_multiply(node->orientation, quat, node->orientation);
  }

  /* printf("translating %f %f %f\n", p[0], p[1], p[2]); */
  node_mark_for_recalc(node);
}

int node_needs_recalc(struct node *node) {
  return (node->parent && node->parent->needs_recalc) || node->needs_recalc;
}

vec3 *node_world(struct node *node) {
  assert(!node_needs_recalc(node));
  return &node->mat[M_X];
}

void node_mark_for_recalc(struct node *node) {
  static int j = 0;

  if (node->needs_recalc)
    return;

  node->needs_recalc = 1;

  for (int i = 0; i < node->n_children; ++i)
    node_mark_for_recalc(node->children[i]);
}

static void node_recalc_children(struct node *node) {
  for (int i = 0; i < node->n_children; ++i) {
    node_recalc(node->children[i]);
  }
}

int node_recalc(struct node *node) {
  assert(node);

  if (node->parent && node_needs_recalc(node->parent))
    node_recalc(node->parent);

  if (!node_needs_recalc(node)) {
    node_recalc_children(node);
    return 0;
  }

  node->needs_recalc = 0;

  mat4_create_transform(node->pos, node->scale, node->orientation, node->mat);

  if (node->parent) {
    assert(!node->parent->needs_recalc);
    mat4_multiply(node->parent->mat, node->mat, node->mat);
  }

  if (node->custom_update)
    node->custom_update(node);

  node_recalc_children(node);

  return 1;
}


void node_attach(struct node *node, struct node *to) {
  assert(to->n_children <= MAX_NODE_CHILDREN);

  node->parent = to;
  to->children[to->n_children++] = node;
}

void node_forward(struct node *node, float *dir) {
  float movement[3] = {0};
  float q[4] = {0};
  quat_inverse(node->orientation, q);
  quat_multiply_vec3(q, dir, movement);
  vec3_add(node->pos, movement, node->pos);
  /* printf("dir %f %f %f\nmovement %f %f %f\nquat %f %f %f %f\n", */
  /*        dir[0], dir[1], dir[2], */
  /*        movement[0], movement[1], movement[2], */
  /*        node->orientation[0], node->orientation[1], */
  /*        node->orientation[2], node->orientation[3]); */
  node_mark_for_recalc(node);
}
