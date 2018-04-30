
#ifndef POLYADVENT_NODE_H
#define POLYADVENT_NODE_H

#define MAX_NODE_CHILDREN 4

struct node {
  float pos[3];
  float rot[3];
  float scale[3];
  float mat[16];
  float orientation[4];
  char *label;
  int needs_recalc;
  int n_children;
  int mirrored; // TODO: make camera type
  struct node* parent;
  struct node* children[MAX_NODE_CHILDREN];
  // TODO quaternion rot/orientation
};

int node_recalc(struct node *root);
void node_attach(struct node *node, struct node *to);
void node_mark_for_recalc(struct node *node);
struct node *node_init(struct node *node);
void node_translate(struct node *node, float *p);

#endif /* POLYADVENT_NODE_H */
