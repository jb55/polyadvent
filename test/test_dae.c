
#include "animation.h"
#include "util.h"
#include "debug.h"
#include <assert.h>
#include <stdio.h>

#include "dae.h"

int main(int argc, char *argv[])
{
    struct pose *pose;
    struct joint *joint;
    struct node *node;
    struct model model;
    struct make_geometry geom;
    int nposes;

    init_node_manager();
    init_model(&model);
    init_make_geometry(&geom);

    load_model("data/models/pirate-officer.dae", &model, &geom);
    assert(model.nposes == 1);
    pose = &model.poses[0];

    debug("pose->nweights %d\n", pose->nweights);
    assert(pose->nweights == 389);
    assert(approxeq(pose->weights[0], 0.05213558));
    debug("pose last weight %f\n", pose->weights[388]);
    assert(approxeq(pose->weights[388], 0.01394611));

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
