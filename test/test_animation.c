
#include "animation.h"
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

    printf("joint0 mat0 -> %f\n", joint->mat[0]);
    assert(joint->mat[0] == 0.9998971);

    return 0;
}
