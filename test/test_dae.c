
#include "animation.h"
#include "util.h"
#include "ply.h"
#include "mdl.h"
#include "vec3.h"
#include "debug.h"
#include <assert.h>
#include <stdio.h>

#include "dae.h"

static void test_load_pose(struct model *model)
{
    printf("test_load_pose\n");
    assert(model->nposes == 1);
    struct pose *pose = &model->poses[0];

    debug("pose->nweights %d\n", pose->nweights);
    assert(pose->nweights == 389);
    assert(approxeq(pose->weights[0], 0.05213558));
    debug("pose last weight %f\n", pose->weights[388]);
    assert(approxeq(pose->weights[388], 0.01394611));

    assert(pose->njoints == 11);

    struct joint *joint = &pose->joints[0];
    struct node *node = get_node(&joint->node_id);
    assert(node);
    assert(approxeq(node->mat[0], 0.999897));

    joint = &pose->joints[1];
    node = get_node(&joint->node_id);
    assert(node);
    assert(approxeq(node->mat[1], -0.01434187));

    assert(pose->joints[0].n_children_ids == 3);
}

static void test_save_mdl()
{
    printf("test_save_mdl\n");
    struct model model, model2;
    struct mdl_geometry mg, mg2;

    init_model(&model);
    init_mdl_geometry(&mg);
    init_model(&model2);
    init_mdl_geometry(&mg2);

    const char *filename = "data/models/pirate_officer.ply";
    parse_ply_with_mkgeom(filename, &mg);

    FILE *out = fopen("/tmp/test.mdl", "wb");
    save_mdl(out, &model, &mg);
    fclose(out);

    out = fopen("/tmp/test.mdl", "rb");
    load_mdl(out, &model2, &mg2);

    assert(model.nposes == model2.nposes);
    assert(vec3_approxeq(mg.min, mg2.min));
    assert(vec3_approxeq(mg.max, mg2.max));
}

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

    load_dae("data/models/pirate-officer.dae", &model, &geom);

    test_load_pose(&model);
    test_save_mdl(&model);

    return 0;
}