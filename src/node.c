

#include "node.h"
#include "mat_util.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>

struct node *node_init(struct node *node) {
  mat4_id(node->mat);
  vec3_all(node->pos, 0);
  vec3_all(node->rot, 0);
  vec3_all(node->scale, 1.0);
  quat_id(node->orientation);
  node->n_children = 0;
  for (int i = 0; i < MAX_NODE_CHILDREN; ++i)
    node->children[i] = NULL;
  node->flags = 0;
  node->parent = NULL;
  node->label = "unknown";
  node->needs_recalc = 0;
  node->custom_update = 0;
  node_mark_for_recalc(node);

  return node;
}

void node_scale(struct node *node, float val) {
    vec3_scale(node->scale, val, node->scale);
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

int node_needs_recalc(struct node *node)
{
  return (node->parent && node->parent->needs_recalc) || node->needs_recalc;
}

vec3 *node_world(struct node *node) {
    node_recalc(node);
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
  float rot[9] = {1.0};

  if (node->parent && node_needs_recalc(node->parent))
    node_recalc(node->parent);

  if (!node_needs_recalc(node)) {
    node_recalc_children(node);
    return 0;
  }

  node->needs_recalc = 0;

  if (!node->custom_update) {
      quat_to_mat3(node->orientation, rot);
      mat4_create_transform(node->pos, node->scale, rot, node->mat);

      if (node->parent) {
          assert(!node->parent->needs_recalc);
          mat4_multiply(node->parent->mat, node->mat, node->mat);
      }

  }
  else {
      node->custom_update(node);
  }

  node_recalc_children(node);

  return 1;
}


int node_detach(struct node *node, struct node *from) {
    for (int i = 0; i < from->n_children; i++) {
        if (from->children[i] == node) {
            memmove(from->children[i], from->children[i+1],
                    from->n_children - i - 1);
            from->n_children--;
            return 1;
        }
    }
    return 0;
}

void node_attach(struct node *node, struct node *to) {
  assert(to->n_children <= MAX_NODE_CHILDREN);

  node->parent = to;
  to->children[to->n_children++] = node;
}

void node_forward(struct node *node, float *dir) {
    vec3_forward(node->pos, node->orientation, dir, node->pos);
    node_mark_for_recalc(node);
}
