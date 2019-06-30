
#ifndef POLYADVENT_ANIMATION_H
#define POLYADVENT_ANIMATION_H

#include "common.h"
#include "node.h"

#define MAX_JOINT_CHILDREN 4
#define MAX_JOINTS 16
#define JOINT_LABEL_SIZE 8

struct joint
{
    int id;
    int children_ids[MAX_JOINT_CHILDREN];
    int n_children_ids;
    node_id node_id;
};


struct pose
{
    struct joint joints[MAX_JOINTS];
    int njoints;
};


void load_poses(const char *filename, struct pose *poses, int *nposes);

void init_joint(struct joint *joint);
void init_pose(struct pose *pose);


#endif /* POLYADVENT_ANIMATION_H */
