
#ifndef POLYADVENT_ANIMATION_H
#define POLYADVENT_ANIMATION_H

#include "common.h"

#define MAX_JOINT_CHILDREN 3
#define MAX_JOINTS 16

struct joint
{
    int children[MAX_JOINT_CHILDREN];
    int id;
    float mat[MAT4_ELEMS];
};



struct pose
{
    struct joint joints[MAX_JOINTS];
    int njoints;
};


void load_poses(const char *filename, struct pose *poses, int *nposes);

void init_joint(struct joint *joint);


#endif /* POLYADVENT_ANIMATION_H */
