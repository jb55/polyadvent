
#include "animation.h"
#include "util.h"
#include "ply.h"
#include "mdl.h"
#include "vec3.h"
#include "debug.h"
#include <assert.h>
#include <stdio.h>

#include "dae.h"

static void test_load_pose()
{
    printf("test_load_pose\n");

    struct model model;
    struct make_geometry mkgeom;
    init_model(&model);
    init_make_geometry(&mkgeom);

    load_dae("data/models/pirate_officer.dae", &model, &mkgeom);

    assert(mkgeom.indices);

    assert(model.nposes == 1);
    struct pose *pose = &model.poses[0];

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

    free_make_geometry(&mkgeom);
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

    struct make_geometry *mk1 = &mg.mkgeom;
    struct make_geometry *mk2 = &mg.mkgeom;

    // TODO: test more models
    load_dae("data/models/pirate_officer.dae", &model, mk1);

    const char *tmpfile = "/tmp/test.mdl";
    save_mdl(tmpfile, &model, &mg);
    load_mdl(tmpfile, &model2, &mg2);

    debug("poses %d %d\n", model.nposes, model2.nposes);

    // TODO: mdl poses
    /* assert(model.nposes == model2.nposes); */

    assert(vec3_approxeq(mg.min, mg2.min));
    assert(vec3_approxeq(mg.max, mg2.max));
    assert(mk1->num_verts == mk2->num_verts);
    assert(mk1->num_indices == mk2->num_indices);
    assert(mk1->num_uv_components == mk2->num_uv_components);
    assert(mk1->num_joint_ids == mk2->num_joint_ids);

    assert(memeq(mk1->vertices, mk2->vertices,
                 mk1->num_verts * 3, mk2->num_verts * 3));

    assert(memeq(mk1->colors, mk2->colors,
                 mk1->num_verts * 3, mk2->num_verts * 3));

    assert(memeq(mk1->normals, mk2->normals,
                 mk1->num_verts * 3, mk2->num_verts * 3));

    assert(memeq(mk1->indices, mk2->indices,
                 mk1->num_indices, mk2->num_indices));

    assert(memeq(mk1->tex_coords, mk2->tex_coords,
                 mk1->num_verts * mk2->num_uv_components,
                 mk2->num_verts * mk2->num_uv_components));

    assert(memeq(mk1->joint_ids, mk2->joint_ids,
                 mk1->num_joint_ids, mk2->num_joint_ids));

    assert(memeq(mk1->joint_weights, mk2->joint_weights,
                 mk1->num_verts * 3, mk2->num_verts * 3));
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

    test_load_pose();
    test_save_mdl();

    return 0;
}
