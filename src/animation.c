
#include <stdio.h>
#include "xml.h"
#include "util.h"
#include "animation.h"

enum dae_state {
    PARSING_START,
    PARSING_NODE,
    PARSING_POSE,
    PARSING_JOINT,
    PARSING_JOINT_MATRIX,
};

struct dae_data {
    int opened;
    int state;
    FILE *dae_file;
    struct pose *poses;
    int *nposes;
};

void init_joint(struct joint *joint)
{
    memset(joint->children, -1, sizeof(joint->children));
    joint->id = -1;
    mat4_id(joint->mat);
}

void init_pose(struct pose *pose)
{
    pose->njoints = 0;
}

static void parse_joint(const char *t, int id, struct joint *joint)
{
    float *m = joint->mat;
    init_joint(joint);
    joint->id = id;
    printf(" parsing joint %d: %s\n", id, t);

    sscanf(t, "%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f",
           &m[0],  &m[1],  &m[2],  &m[3],
           &m[4],  &m[5],  &m[6],  &m[7],
           &m[8],  &m[9],  &m[10], &m[11],
           &m[12], &m[13], &m[14], &m[15]
           );
}

static void dae_tag_start(struct xmlparser *x, const char *t, size_t tl)
{
    struct dae_data *data = (struct dae_data*)x->user_data;
    data->opened++;

    if (streq(t, "node"))
        data->state = PARSING_NODE;
    else if (data->state == PARSING_JOINT && streq(t, "matrix"))
        data->state = PARSING_JOINT_MATRIX;
    else
        return;

    printf("\n");
    for (int i = 0; i < data->opened; i++) {
        putchar(' ');
    }
    printf("%s", t);
}

static void dae_tag_end(struct xmlparser *x, const char *t, size_t tl, int what)
{
    struct dae_data *data = (struct dae_data*)x->user_data;

    if (data->state == PARSING_NODE)
        data->state = PARSING_START;

    data->opened--;
}

static void dae_tagbody(struct xmlparser *x, const char *d, size_t dl)
{
    struct dae_data *data = (struct dae_data*)x->user_data;

    if (data->state == PARSING_JOINT_MATRIX) {
        assert(*data->nposes);
        struct pose *pose = &data->poses[*data->nposes - 1];
        struct joint *joint = &pose->joints[pose->njoints];
        parse_joint(d, pose->njoints, joint);
        pose->njoints++;
        data->state = PARSING_POSE;
    }
}

static int dae_getc(struct xmlparser *x)
{
    struct dae_data *data = (struct dae_data*)x->user_data;
    return fgetc(data->dae_file);
}

void dae_attr(struct xmlparser *x, const char *t, size_t tl,
              const char *a, size_t al, const char *v, size_t vl)
{
    struct dae_data *data = (struct dae_data*)x->user_data;

    if (data->state == PARSING_NODE
        && streq(a, "id")
        && streq(v, "Armature"))
    {
        struct pose *pose = &data->poses[(*data->nposes)++];
        data->state = PARSING_POSE;
        init_pose(pose);
    }
    else if (data->state == PARSING_NODE
             && streq(a, "type")
             && streq(v, "JOINT"))
    {
        data->state = PARSING_JOINT;
    }
}

void load_poses(const char *filename, struct pose *poses, int *nposes)
{
    *nposes = 0;

    struct xmlparser x = {0};
    struct dae_data data = {
      .opened = 0,
      .state  = PARSING_START,
      .poses  = poses,
      .nposes = nposes,
    };

    data.dae_file = fopen("data/models/pirate-officer.dae", "rb");
    if (data.dae_file == NULL)
        exit(1);

    x.user_data = &data;
    x.xmltagstart = dae_tag_start;
    x.xmltagend   = dae_tag_end;
    x.xmlattr     = dae_attr;
    x.xmldata     = dae_tagbody;
    /* x.xmlattrend  = dae_attr_end; */
    x.getnext     = dae_getc;

    xml_parse(&x);

    fclose(data.dae_file);
}
