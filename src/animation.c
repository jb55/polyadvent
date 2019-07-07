
#include <stdio.h>
#include "xml.h"
#include "util.h"
#include "node.h"
#include "debug.h"
#include "animation.h"

void init_joint(struct joint *joint)
{
    joint->id = -1;
    joint->n_children_ids = 0;
    for (int i = 0; i < MAX_JOINT_CHILDREN; i++) {
        joint->children_ids[i] = -1;
    }
    init_id(&joint->node_id);
}

void new_joint(struct joint *joint)
{
    init_joint(joint);
    new_node(&joint->node_id);
}


void init_pose(struct pose *pose)
{
    pose->njoints = 0;
    for (int i = 0; i < MAX_JOINTS; i++) {
        init_joint(&pose->joints[i]);
    }
}


