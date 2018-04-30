
#ifndef POLYADVENT_NODE_H
#define POLYADVENT_NODE_H

struct node {
  float pos[3];
  float rot[3];
  float scale[3];
  float mat[16];
  int needs_recalc;
  int mirrored; // TODO: make camera type
  struct node* children;
  // TODO quaternion rotation
};

int node_recalc(struct node *node);
struct node *node_init(struct node *node);
void node_translate(struct node *node, float *p);

#endif /* POLYADVENT_NODE_H */
