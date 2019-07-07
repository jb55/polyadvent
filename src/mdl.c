
#include "mdl.h"
#include "debug.h"
#include <assert.h>


static inline void write_tag(enum mdl_tag tag, FILE *out)
{
    fwrite(&tag, 1, 1, out);
}

// little endian is assumed...
static inline void write_int(int val, FILE *out)
{
    fwrite(&val, sizeof(val), 1, out);
}

static inline void write_nfloats(float *floats, int num_floats, FILE *out)
{
    fwrite(floats, sizeof(*floats), num_floats, out);
}

static inline void write_floats(float *floats, int num_floats, FILE *out)
{
    write_int(num_floats, out);
    write_nfloats(floats, num_floats, out);
}

static inline void write_ints(u32 *ints, int num_ints, FILE *out)
{
    write_int(num_ints, out);
    fwrite(ints, sizeof(*ints), num_ints, out);
}

void init_mdl_geometry(struct mdl_geometry *lgeom)
{
    init_make_geometry(&lgeom->mkgeom);
    memset(lgeom->min, 0, sizeof(lgeom->min));
    memset(lgeom->max, 0, sizeof(lgeom->max));
}

void save_mdl(FILE *out, struct model *model, struct mdl_geometry *lgeom)
{
    struct make_geometry *mkgeom = &lgeom->mkgeom;
    assert(mkgeom->vertices);
    assert(mkgeom->normals);

    for (int i = 0; i < N_MDL_TAGS; i++) {
        switch ((enum mdl_tag)i) {
        case MDL_POSITION:
            write_tag(MDL_POSITION, out);
            write_floats(mkgeom->vertices, mkgeom->num_verts * 3, out);
            break;

        case MDL_NORMAL:
            write_tag(MDL_NORMAL, out);
            write_floats(mkgeom->normals, mkgeom->num_verts * 3, out);
            break;

        case MDL_COLOR:
            if (mkgeom->colors) {
                write_tag(MDL_COLOR, out);
                write_floats(mkgeom->colors, mkgeom->num_verts * 3, out);
            }
            break;

        case MDL_INDEX:
            write_tag(MDL_INDEX, out);
            write_ints(mkgeom->indices, mkgeom->num_indices , out);
            /* debug("writing out %d indices\n", mkgeom->num_indices); */
            break;

        case MDL_JOINT_IDS:
            if (mkgeom->joint_ids) {
                write_tag(MDL_JOINT_IDS, out);
                write_ints(mkgeom->joint_ids, mkgeom->num_joint_ids, out);
            }
            break;

        case MDL_JOINT_WEIGHTS:
            if (mkgeom->joint_weights) {
                write_tag(MDL_JOINT_WEIGHTS, out);
                write_floats(mkgeom->joint_weights,
                             mkgeom->num_verts * 3, out);
            }
            break;

        case MDL_TEX_COORD:
            if (mkgeom->tex_coords) {
                write_tag(MDL_TEX_COORD, out);
                assert(mkgeom->num_uv_components == 2);
                write_floats(mkgeom->tex_coords,
                             mkgeom->num_verts * mkgeom->num_uv_components,
                             out);
            }
            break;

        case MDL_MAX:
            write_tag(MDL_MAX, out);
            write_nfloats(lgeom->max, 3, out);
            /* debug("write max: %f %f %f\n", */
            /*       lgeom->max[0], lgeom->max[1], lgeom->max[2]); */
            break;

        case MDL_MIN:
            write_tag(MDL_MIN, out);
            write_nfloats(lgeom->min, 3, out);

            /* debug("write min: %f %f %f\n", */
            /*       lgeom->min[0], lgeom->min[1], lgeom->min[2]); */
            break;

        case N_MDL_TAGS:
            assert(!"this shouldn't happend"); break;
        }
    }

    assert(mkgeom->indices);
}

static inline enum mdl_tag read_tag(FILE *stream)
{
    enum mdl_tag tag = 0;
    int read = fread(&tag, 1, 1, stream);
    /* debug("read_tag %d\n", tag); */
    assert(read == 1 || feof(stream));
    assert(tag != 0 || feof(stream));
    return tag;
}

static inline int read_int(FILE *stream)
{
    int val = 0;
    int read = fread(&val, sizeof(val), 1, stream);
    assert(read == 1);
    assert(val != 0);
    return val;
}

#define MAX_MDL_ARRAY 128000 // only enforced at dev time for sanity

static inline int read_nfloats(FILE *stream, int n, float *floats)
{
    return fread(floats, sizeof(float), n, stream);
}

static int read_floats(FILE *stream, float **floats)
{
    int nfloats = read_int(stream);
    assert(nfloats != 0);
    assert(nfloats < MAX_MDL_ARRAY); // it's a low poly game...
    *floats = malloc(nfloats*sizeof(float));
    int nread = read_nfloats(stream, nfloats, *floats);
    assert(nread == nfloats);
    return nread;
}

static int read_ints(FILE *stream, u32 **ints)
{
    int nints = read_int(stream);
    assert(nints != 0);
    assert(nints < MAX_MDL_ARRAY); // it's a low poly game...
    *ints = malloc(nints*sizeof(u32));
    int nread = fread(*ints, sizeof(u32), nints, stream);
    assert(nread == nints);
    return nread;
}

void load_mdl(FILE *in, struct model *model, struct mdl_geometry *lgeom)
{
    struct make_geometry *mkgeom = &lgeom->mkgeom;
    int num;

    while (1) {
        enum mdl_tag tag = read_tag(in);
        if (feof(in))
            break;
        assert(tag < N_MDL_TAGS);

        switch (tag) {
        case MDL_POSITION:
            mkgeom->num_verts = read_floats(in, &mkgeom->vertices) / 3;
            break;

        case MDL_NORMAL:
            num = read_floats(in, &mkgeom->normals);
            assert(num == mkgeom->num_verts * 3);
            break;

        case MDL_COLOR:
            num = read_floats(in, &mkgeom->colors);
            assert(num == mkgeom->num_verts * 3);
            break;

        case MDL_INDEX:
            mkgeom->num_indices = read_ints(in, &mkgeom->indices);
            /* debug("reading in %d indices\n", mkgeom->num_indices); */
            break;

        case MDL_TEX_COORD:
            num = read_floats(in, &mkgeom->tex_coords);
            assert(num == mkgeom->num_verts * 2);
            break;

        case MDL_JOINT_IDS:
            mkgeom->num_joint_ids = read_ints(in, &mkgeom->joint_ids);
            break;

        case MDL_JOINT_WEIGHTS:
            num = read_floats(in, &mkgeom->joint_weights);
            assert(num == mkgeom->num_verts * 3);
            break;

        case MDL_MIN:
            read_nfloats(in, 3, lgeom->min);
            /* debug("read min: %f %f %f\n", */
            /*       lgeom->min[0], lgeom->min[1], lgeom->min[2]); */
            break;

        case MDL_MAX:
            read_nfloats(in, 3, lgeom->max);
            /* debug("read max: %f %f %f\n", */
            /*       lgeom->max[0], lgeom->max[1], lgeom->max[2]); */
            break;

        case N_MDL_TAGS: assert(!"this shouldn't happend"); break;
        }
    }
}
