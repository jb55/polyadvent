
#include "xml.h"
#include "util.h"
#include "animation.h"
#include "debug.h"
#include "dae.h"

enum dae_state {
    PARSING_START,
    PARSING_NODE,
    PARSING_FLOAT_ARRAY,
    PARSING_POSE,
    PARSING_JOINT,
    PARSING_JOINT_MATRIX,
    PARSING_WEIGHTS,
    PARSING_POSITIONS,
    PARSING_COLORS,
    PARSING_COLOR_INDEX,
    PARSING_TRIANGLES,
    PARSING_INDICES,
    PARSING_TEX_COORDS,
    PARSING_NORMALS,
};

enum semantic_type {
    SEM_VERTEX,
    SEM_NORMAL,
    SEM_COLOR,
    SEM_TEXCOORD,
    N_SEMTYPES
};

struct dae_data {
    int node_level;
    int state;
    FILE *dae_file;
    struct model *model;
    struct make_geometry *geom;

    u32 *indices;
    float *colors;
    float *normals;

    int n_indices;
    int prev_tris;
    int n_tris;

    enum semantic_type current_sem;
    int offsets[N_SEMTYPES];
    int semflags; // 1 << semantic_type

    int counter;
    char current_name[JOINT_LABEL_SIZE];
};

static int n_attrs(int semflags)
{
    int count = 0;
    for (int i = 0; i < N_SEMTYPES; i++)
        count += (semflags & (1 << i)) == (1 << i);
    return count;
}

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
    else if (streq(t, "triangles")) {
        data->state = PARSING_TRIANGLES;
    }
    else if (data->state == PARSING_TRIANGLES
             && streq(t, "p")) {
        data->state = PARSING_INDICES;
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


static void parse_array_with_offset(const char *d, int dl, u8 **elems,
                                    const char *fmt, int nelems,
                                    int elem_size, int elem_offset)
{
    u8 buf[8];
    int i;

    assert(elem_size <= 8);
    assert(nelems > 0);

    *elems = realloc(*elems, nelems * elem_size);

    const char *p = d;

    for (i = elem_offset; i < nelems; i++) {
        sscanf(p, fmt, &buf);
        memcpy(*elems + i*elem_size, &buf, elem_size);

        assert(p-d < XML_BUFSIZ);
        assert (p < d+dl);
        while (p < d+dl) {
            if (*(p++) == ' ') {
                break;
            }
        }
    }

    assert(nelems == i);
}

static void parse_array(const char *d, int dl, u8 **elems,
                        const char *fmt, int nelems, int elem_size)
{
    assert(*elems == NULL);
    parse_array_with_offset(d, dl, elems, fmt, nelems, elem_size, 0);
}

static void parse_floats(const char *d, int dl, float **floats, int nfloats)
{
    parse_array(d, dl, (u8**)floats, "%f", nfloats, sizeof(float));
}

static void parse_ints_with_offset(const char *d, int dl, u32 **ints,
                                   int nints, int offset)
{
    parse_array_with_offset(d, dl, (u8**)ints, "%d", nints,
                            sizeof(u32), offset);
}

static void parse_ints(const char *d, int dl, u32 **ints, int nints)
{
    parse_ints_with_offset(d, dl, ints, nints, 0);
}


static void parse_weights(struct dae_data *data, const char *d, int dl)
{
    assert(data->counter != -1);
    float *weights = NULL;
    parse_floats(d, dl, &weights, data->counter);
    struct model *model = data->model;
    struct pose *pose = &model->poses[model->nposes];
    assert(pose);
    pose->nweights = data->counter;
    pose->weights = weights;
    data->state = PARSING_POSE;
    data->counter = -1;
}

static void parse_joint_matrix(struct dae_data *data, const char *d,
                               int dl)
{
    assert(data->model->nposes);

    struct model *model = data->model;
    struct pose *pose = &model->poses[model->nposes - 1];
    assert(pose);

    struct joint *joint = &pose->joints[pose->njoints];
    assert(joint);

    struct node *node = new_node(&joint->node_id);
    assert(node);
    assert((int64_t)joint->node_id.uuid != -1);
    assert(&joint->node_id == &pose->joints[pose->njoints].node_id);

    parse_joint(d, pose->njoints, joint);
    node_set_label(node, data->current_name);
    joint->children_ids[0] = data->node_level;
    pose->njoints++;
    data->state = PARSING_POSE;
}

static void reset_parse_state(struct dae_data *data)
{
    data->state = PARSING_POSE;
    data->counter = -1;
}

static void parse_vertices(struct dae_data *data, const char *d, int dl)
{
    assert(data->counter != -1);
    parse_floats(d, dl, &data->geom->vertices, data->counter);
    data->geom->num_verts = data->counter / 3;
    reset_parse_state(data);
}

static void parse_normals(struct dae_data *data, const char *d, int dl)
{
    assert(data->counter != -1);
    parse_floats(d, dl, &data->normals, data->counter);
    reset_parse_state(data);
}

static void parse_colors(struct dae_data *data, const char *d, int dl)
{
    assert(data->counter != -1);
    parse_floats(d, dl, &data->colors, data->counter);
    reset_parse_state(data);
}

static void parse_indices(struct dae_data *data, const char *d, int dl)
{
    assert(data->counter != -1);
    int nattrs = n_attrs(data->semflags);
    // debug("data->counters %d\n", data->counter);
    // debug("prev_tris %d n_tris %d counter %d nattrs %d\n",
    //       data->prev_tris,
    //       data->n_tris,
    //       data->counter,
    //       nattrs);

    parse_ints_with_offset(d, dl, &data->indices,
                           data->n_tris * 3 * nattrs,
                           data->prev_tris * 3 * nattrs );
    reset_parse_state(data);
}

static void dae_tagbody(struct xmlparser *x, const char *d, size_t dl)
{
    static int count = 0;
    struct dae_data *data = (struct dae_data*)x->user_data;

    if (data->state == PARSING_JOINT_MATRIX) {
        parse_joint_matrix(data, d, dl);
    }
    else if (data->geom && data->state == PARSING_POSITIONS) {
        parse_vertices(data, d, dl);
    }
    else if (data->geom && data->state == PARSING_WEIGHTS) {
        parse_weights(data, d, dl);
    }
    else if (data->geom && data->state == PARSING_NORMALS) {
        parse_normals(data, d, dl);
    }
    else if (data->geom && data->state == PARSING_COLORS) {
        parse_colors(data, d, dl);
    }
    else if (data->geom && data->state == PARSING_INDICES) {
        parse_indices(data, d, dl);
    }
    /* else if (data->state == PARSING_TEX_COORDS) { */
    /*     parse_tex_coords(data, d, dl); */
    /* } */
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

        struct model *model = data->model;
        struct pose *pose = &model->poses[model->nposes++];
        data->state = PARSING_POSE;
        init_pose(pose);
    }
    else if (data->state == PARSING_FLOAT_ARRAY && streq(a, "id")) {
        if (contains(v, "skin-weights-array")) {
            data->state = PARSING_WEIGHTS;
        }
        else if (contains(v, "positions-array")) {
            data->state = PARSING_POSITIONS;
        }
        else if (contains(v, "normals-array")) {
            data->state = PARSING_NORMALS;
        }
        else if (contains(v, "mesh-colors")) {
            data->state = PARSING_COLORS;
        }
        else if (contains(v, "mesh-map")) {
            data->state = PARSING_TEX_COORDS;
        }
    }
    // this is just for sanity checking the order of things
    // if this ever asserts, we need to do proper offset parsing
    else if (data->state == PARSING_TRIANGLES && streq(a, "semantic")) {
        if (streq(v, "VERTEX"))
            data->current_sem = SEM_VERTEX;
        else if (streq(v, "NORMAL"))
            data->current_sem = SEM_NORMAL;
        else if (streq(v, "TEXCOORD"))
            data->current_sem = SEM_TEXCOORD;
        else if (streq(v, "COLOR"))
            data->current_sem = SEM_COLOR;
        else
            assert(!"unhandled triangle attribute index");
    }
    else if (data->state == PARSING_TRIANGLES
             && streq(a, "offset")) {
        int offset = atoi(v);
        assert((int)data->current_sem != -1);

        data->semflags |= 1 << data->current_sem;
        data->offsets[data->current_sem] = offset;
    }
    else if (streq(a, "count")) {
        data->counter = atoi(v);
        if (data->state == PARSING_TRIANGLES) {
            data->prev_tris = data->n_tris;
            data->n_tris += data->counter;
        }
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

static void print_indices(struct dae_data *data)
{
    int nattrs = n_attrs(data->semflags);
    for (int i = 0; i < data->n_tris * 3 * nattrs; i++) {
        printf("%d ", data->indices[i]);
    }
    printf("\n");
}

static void process_indices(struct dae_data *data)
{
    int nattrs = n_attrs(data->semflags);
    int num_indices = data->n_tris * 3;
    bool has_color = data->semflags & (1 << SEM_COLOR);

    assert(num_indices);

    data->geom->num_indices = num_indices;

    if (has_color) {
        assert(data->offsets[SEM_COLOR] != -1);
        data->geom->colors = calloc(num_indices, sizeof(float) * 3);
    }

    assert(data->semflags & (1 << SEM_NORMAL));
    assert(data->semflags & (1 << SEM_VERTEX));

    data->geom->normals = calloc(num_indices, sizeof(float) * 3);
    data->geom->indices = calloc(num_indices, sizeof(int));

    // debug("offsets v:%d n:%d c:%d t:%d\n",
    //       data->offsets[SEM_VERTEX],
    //       data->offsets[SEM_NORMAL],
    //       data->offsets[SEM_COLOR],
    //       data->offsets[SEM_TEXCOORD]);

    // debug("data->n_indices %d num_indices %d nattrs %d\n",
    //       data->n_indices,
    //       num_indices,
    //       nattrs);

    /* print_indices(data); */

    assert(data->n_indices % nattrs == 0);

    for (int i = 0; i < num_indices; i++) {
        int v = i * nattrs;

        int cind = 0;
        int vind = data->indices[v + data->offsets[SEM_VERTEX]];
        int nind = data->indices[v + data->offsets[SEM_NORMAL]];

        assert(vind < data->geom->num_verts);

        if (has_color) {
            cind = data->indices[v + data->offsets[SEM_COLOR]];
        }

        assert(cind < 50000);

        data->geom->indices[i] = vind;

        // if (i == 0)
        //     printf("first vert %f %f %f\n",
        //         (&data->geom->vertices[vind*3])[0],
        //         (&data->geom->vertices[vind*3])[1],
        //         (&data->geom->vertices[vind*3])[2]);
        for (int j = 0; j < 3; j++) {
            int ind = (i*3)+j;

            if (has_color) {
                data->geom->colors[ind]  = (&data->colors[cind*3])[j];
            }

            data->geom->normals[ind] = (&data->normals[nind*3])[j];
        }
    }

    assert(data->indices);
    if (data->indices)
        free(data->indices);

    assert(data->normals);
    if (data->normals)
        free(data->normals);

    assert(data->colors);
    if (data->colors)
        free(data->colors);
}

void load_dae(const char *filename, struct model *model,
              struct make_geometry *geom)
{
    struct xmlparser x = {0};
    struct dae_data data = {
      .node_level   = 0,
      .state        = PARSING_START,
      .model        = model,
      .geom         = geom,
      .indices      = NULL,
      .colors       = NULL,
      .normals      = NULL,
      .n_indices    = 0,
      .n_tris       = 0,
      .prev_tris    = 0,
      .semflags     = 0,
      .current_sem  = -1,
      .counter      = -1
    };

    for (int i = 0; i < N_SEMTYPES; i++)
        data.offsets[i] = -1;

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

    for (int i = 0; i < model->nposes; i++) {
        struct pose *pose = &model->poses[i];
        process_joint_children(pose->joints, pose->njoints);
    }

    if (geom)
        process_indices(&data);

    fclose(data.dae_file);
}
