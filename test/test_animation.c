
#include "animation.h"
#include "util.h"
#include <assert.h>
#include <stdio.h>


int main(int argc, char *argv[])
{
    struct pose *pose;
    struct joint *joint;
    struct pose poses[4];
    int nposes;

    load_poses("data/models/pirate-officer.dae", poses, &nposes);
    assert(nposes == 1);

    pose = &poses[0];
    assert(pose->njoints == 11);

    joint = &pose->joints[0];
    assert(approxeq(joint->mat[0], 0.999897));

    joint = &pose->joints[1];
    assert(approxeq(joint->mat[1], -0.01434187));

    assert(pose->joints[0].nchildren == 3);

    return 0;
}
