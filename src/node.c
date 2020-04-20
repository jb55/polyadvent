

#include "node.h"
#include "mat_util.h"
#include "debug.h"
#include "static_resources.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

struct resource_manager node_manager = {0};

static inline struct node *new_uninitialized_node(node_id *id)
{
    return new_resource(&node_manager, id);
}

struct node *new_node(node_id *id)
{
    struct node *n = node_init(new_uninitialized_node(id));
    assert((int64_t)id->uuid != -1);
    /* debug("new node %llu\n", id->uuid); */
    return n;
}

struct node *get_node(node_id *id)
{
    return get_resource(&node_manager, id);
}

void destroy_node(node_id *id)
{
#ifdef DEBUG
    struct node *node = get_node(id);
    debug("destroying node %llu %s\n", id->uuid, node->label);
#endif

    destroy_resource(&node_manager, id);
}

void init_node_manager()
{
    init_resource_manager(&node_manager, sizeof(struct node), 4096,
                          0xFFFF, "node", N_STATIC_NODES);

    for (int i = 0; i < N_STATIC_NODES; i++)
        node_init(&static_nodes()[i]);
}

struct node *node_init(struct node *node) {
  mat4_id(node->mat);
  vec3_all(node->pos, 0);
  vec3_all(node->rot, 0);
  vec3_all(node->scale, 1.0);
  quat_id(node->orientation);
  node->n_children = 0;
  for (int i = 0; i < MAX_NODE_CHILDREN; ++i)
      init_id(&node->children_ids[i]);
  node->flags = 0;
  null_id(&node->parent_id);
  node_set_label(node, "unknown");
  node->needs_recalc = 0;
  node->custom_update = 0;
  node_mark_for_recalc(node);

  return node;
}

int node_set_label(struct node *node, const char *label)
{
    strncpy(node->label, label, sizeof(node->label)-1);
    // return 0 if the string is too big and that we've truncated it
    int ok = node->label[sizeof(node->label)-1] == '\0';
    // strncpy won't write this for large strings =/
    node->label[sizeof(node->label)-1] = '\0';
    return ok;
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
    struct node *parent = get_node(&node->parent_id);
    return (parent && parent->needs_recalc) || node->needs_recalc;
}

vec3 *node_world(struct node *node) {

    return &node->mat[M_X];
}

void node_mark_for_recalc(struct node *node) {
  static int j = 0;

  if (node->needs_recalc)
    return;

  node->needs_recalc = 1;

  for (int i = 0; i < node->n_children; ++i) {
      struct node *child = get_node(&node->children_ids[i]);
      if (child)
          node_mark_for_recalc(child);
  }
}

static void node_recalc_children(struct node *node) {
  for (int i = 0; i < node->n_children; ++i) {
      struct node *child = get_node(&node->children_ids[i]);
      if (child)
          node_recalc(child);
  }
}

int node_recalc(struct node *node) {
  assert(node);
  float rot[9] = {1.0};

  struct node *parent = get_node(&node->parent_id);
  if (parent && node_needs_recalc(parent)) {
      node_recalc(parent);
  }

  if (!node_needs_recalc(node)) {
    node_recalc_children(node);
    return 0;
  }

  node->needs_recalc = 0;

  if (!node->custom_update) {
      quat_to_mat3(node->orientation, rot);
      mat4_create_transform(node->pos, node->scale, rot, node->mat);

      if (parent) {
          assert(!parent->needs_recalc);
          mat4_multiply(parent->mat, node->mat, node->mat);
      }

  }
  else {
      debug("custom updating %s\n", node->label);
      node->custom_update(node);
  }

  node_recalc_children(node);

  return 1;
}

int node_detach(struct node *node, struct node *from) {
    for (int i = 0; i < from->n_children; i++) {
        node_id *child_id = &from->children_ids[i];
        struct node *child = get_node(&from->children_ids[i]);
        if (child && child == node) {
            destroy_node(child_id);
            memmove(&from->children_ids[i], &from->children_ids[i+1],
                    sizeof(*from->children_ids) * (from->n_children - i - 1));
            // TODO: test node_detach
            from->n_children--;
            return 1;
        }
    }
    return 0;
}

void node_detach_from_parent(struct node *node)
{
    struct node *parent = get_node(&node->parent_id);

    if (parent)
        node_detach(node, parent);
}

// count the total number of nodes
int node_count(struct node *node)
{
    int c = 1;
    for (int i = 0; i < node->n_children; i++) {
        struct node *child = get_node(&node->children_ids[i]);
        assert(child);
        if (child)
            c += node_count(child);
    }
    return c;
}

void node_attach(struct resource_id *node_id, struct resource_id *to_id)
{
    struct node *node = get_node(node_id);
    struct node *to   = get_node(to_id);

    assert(node);
    assert(to && to->n_children <= MAX_NODE_CHILDREN);

    node->parent_id = *to_id;
    assert(node->parent_id.uuid == to_id->uuid);
    to->children_ids[to->n_children++] = *node_id;
}

void node_forward(struct node *node, float *dir) {
    vec3_forward(node->pos, node->orientation, dir, node->pos);
    node_mark_for_recalc(node);
}
