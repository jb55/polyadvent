
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "xml.h"
#include "util.h"
#include "animation.h"

enum dae_state {
    PARSING_START,
    PARSING_NODE,
    PARSING_FLOAT_ARRAY,
    PARSING_POSE,
    PARSING_JOINT,
    PARSING_JOINT_MATRIX,
    PARSING_WEIGHTS,
};

struct dae_data {
    int node_level;
    int state;
    FILE *dae_file;
    struct pose *poses;
    int *nposes;
    float *weights;
    int nweights;
    char current_name[JOINT_LABEL_SIZE];
};


static void parse_joint(const char *t, int id, struct joint *joint)
{
    struct node *node = get_node(&joint->node_id);
    assert(node);

    float *m = node->mat;
    joint->id = id;
    /* printf(" parsing joint %d: %s\n", id, t); */

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

    if (streq(t, "node")) {
        data->state = PARSING_NODE;
        data->node_level++;
    }
    else if (streq(t, "float_array")) {
        data->state = PARSING_FLOAT_ARRAY;
    }
    else if (data->state == PARSING_JOINT && streq(t, "matrix"))
        data->state = PARSING_JOINT_MATRIX;
    else
        return;
}

static void dae_tag_end(struct xmlparser *x, const char *t, size_t tl, int what)
{
    struct dae_data *data = (struct dae_data*)x->user_data;

    if (streq(t, "node"))
        data->node_level--;

    if (data->state == PARSING_NODE) {
        data->state = PARSING_START;
    }
}

static void dae_tagbody(struct xmlparser *x, const char *d, size_t dl)
{
    static int count = 0;
    struct dae_data *data = (struct dae_data*)x->user_data;
    struct pose *pose;

    if (data->state == PARSING_JOINT_MATRIX) {
        assert(*data->nposes);

        pose = &data->poses[*data->nposes - 1];
        assert(pose);

        struct joint *joint = &pose->joints[pose->njoints];
        assert(joint);

        struct node *node = new_node(&joint->node_id);
        assert(node);
        assert((int64_t)joint->node_id.uuid != -1);
        assert(&joint->node_id == &pose->joints[pose->njoints].node_id);

        parse_joint(d, pose->njoints, joint);
        print_id(&joint->node_id, 1);
        node_set_label(node, data->current_name);
        joint->children_ids[0] = data->node_level;
        pose->njoints++;
        data->state = PARSING_POSE;
    }
    else if (data->state == PARSING_WEIGHTS) {
        pose = &data->poses[*data->nposes];
        assert(data->nweights > 0);
        data->weights = calloc(data->nweights, sizeof(float));

        const char *p = d;
        float val;
        int i;

        for (i = 0; i < data->nweights; i++) {
            sscanf(p, "%f", &val);
            data->weights[i] = val;

            while (p < d+dl) {
                if (*(p++) == ' ')
                    break;
            }
        }

        assert(data->nweights == i);

        pose->nweights = data->nweights;
        pose->weights = data->weights;

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
        && streq(v, "Armature")) {

        struct pose *pose = &data->poses[(*data->nposes)++];
        data->state = PARSING_POSE;
        init_pose(pose);
    }
    else if (data->state == PARSING_FLOAT_ARRAY
             && streq(a, "id")
             && contains(v, "skin-weights-array")) {
        data->state = PARSING_WEIGHTS;
    }
    else if (data->state == PARSING_WEIGHTS
             && streq(a, "count")) {
        data->nweights = atoi(v);
    }
    else if (data->state == PARSING_NODE
             && streq(a, "name")) {
        strncpy(data->current_name, v, sizeof(data->current_name));
    }
    else if (data->state == PARSING_NODE
             && streq(a, "type")
             && streq(v, "JOINT")) {

        data->state = PARSING_JOINT;
    }
}

static void process_joint_children(struct joint *joints, int njoints)
{
    struct joint *joint, *j2;
    for (int i = 0; i < njoints; i++) {
        joint = &joints[i];

        // node level is stored in here on the first parser pass
        int level = joint->children_ids[0];

        for (int j = i+1; j < njoints; j++) {
            j2 = &joints[j];
            if (j2->children_ids[0] == level + 1) {
                /* printf("%s(%d) assigning child %s(%d)\n", */
                /*        joint->name, level, j2->name, j2->children[0]); */

                assert(joint->n_children_ids+1 < MAX_JOINT_CHILDREN);
                joint->children_ids[joint->n_children_ids++] = j;
            }
            else if (j2->children_ids[0] <= level)
                break;
        }
    }
}

void load_poses(const char *filename, struct pose *poses, int *nposes)
{
    *nposes = 0;

    struct xmlparser x = {0};
    struct dae_data data = {
      .node_level = 0,
      .state  = PARSING_START,
      .poses  = poses,
      .nposes = nposes,
      .nweights = -1,
      .weights = NULL
    };

    data.dae_file = fopen(filename, "rb");
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

    for (int i = 0; i < *nposes; i++) {
        struct pose *pose = &poses[i];
        process_joint_children(pose->joints, pose->njoints);
    }


    fclose(data.dae_file);
}

void usage()
{
    printf("usage: compile-model <model.dae> <out.mdl>\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        usage();

    const char *filename = argv[1];
    const char *outfile = argv[2];

    int nposes;
    static struct pose poses[5];

    load_poses(outfile, poses, &nposes);

    FILE *out = fopen(outfile, "wb");
    fprintf(out, "hello, world\n");
    fclose(out);

    return 0;
}
