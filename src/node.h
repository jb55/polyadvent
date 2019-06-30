
#ifndef POLYADVENT_NODE_H
#define POLYADVENT_NODE_H

#define MAX_NODE_CHILDREN 4

enum node_flags {
  NODE_IGNORE_RECALC = 1 << 0
};

struct node {
  float pos[3];
  float rot[3];
  float scale[3];
  float mat[16];
  float orientation[4];
  char label[8];
  int needs_recalc;
  int flags;
  int n_children;
  void (*custom_update)(struct node*);
  void *custom_update_data;
  struct node* parent;
  struct node* children[MAX_NODE_CHILDREN];
};

int node_recalc(struct node *root);
void node_attach(struct node *node, struct node *to);
int node_detach(struct node *node, struct node *from);
void node_detach_from_parent(struct node *node);
void node_mark_for_recalc(struct node *node);
struct node *node_init(struct node *node);
void node_translate(struct node *node, float *p);
void node_forward(struct node *node, float *p);
void node_forward_dir(struct node *node, float *orientation, float *p);
void node_rotate(struct node *node, float *p);
void node_scale(struct node *node, float val);
int node_count(struct node *node);
float *node_world(struct node *node);
int node_set_label(struct node *node, const char *label);

#endif /* POLYADVENT_NODE_H */
