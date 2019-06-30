
#include "animation.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    struct pose *pose;
    struct joint *joint;
    struct node *node;
    struct pose poses[4];
    int nposes;

    init_node_manager();

    for (int i = 0; i < (int)ARRAY_SIZE(poses); i++) {
        init_pose(&poses[i]);
    }

    load_poses("data/models/pirate-officer.dae", poses, &nposes);
    assert(nposes == 1);

    pose = &poses[0];
    assert(pose->njoints == 11);

    joint = &pose->joints[0];
    node = get_node(&joint->node_id);
    assert(node);
    assert(approxeq(node->mat[0], 0.999897));

    joint = &pose->joints[1];
    node = get_node(&joint->node_id);
    assert(node);
    assert(approxeq(node->mat[1], -0.01434187));

    assert(pose->joints[0].n_children_ids == 3);

    return 0;
}
