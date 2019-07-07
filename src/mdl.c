
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

static inline void write_floats(float *floats, int num_floats, FILE *out)
{
    write_int(num_floats, out);
    fwrite(floats, sizeof(*floats), num_floats, out);
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


    if (mkgeom->normals) {
    }

    if (mkgeom->colors) {
    }

    for (int i = 0; i < N_MDL_TAGS; i++) {
        write_tag(MDL_NORMAL, out);
        switch ((enum mdl_tag)i) {
        case MDL_POSITION:
            write_floats(mkgeom->vertices, mkgeom->num_verts, out);
            break;
        case MDL_NORMAL:
            write_floats(mkgeom->normals, mkgeom->num_verts, out);
            break;
        case MDL_COLOR:
            write_floats(mkgeom->colors, mkgeom->num_verts, out);
            break;
        case MDL_INDEX:
            write_ints(mkgeom->indices, mkgeom->num_indices, out);

        case MDL_JOINT_IDS:
        case MDL_JOINT_WEIGHTS:
        case MDL_TEX_COORD:
        case MDL_MAX:
        case MDL_MIN:
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
        debug("tag %d\n", tag);
        assert(tag < N_MDL_TAGS);

        switch (tag) {
        case MDL_POSITION:
            mkgeom->num_verts = read_floats(in, &mkgeom->vertices);
            break;

        case MDL_NORMAL:
            num = read_floats(in, &mkgeom->normals);
            assert(num == mkgeom->num_verts);
            break;

        case MDL_COLOR:
            num = read_floats(in, &mkgeom->colors);
            assert(num == mkgeom->num_verts);
            break;

        case MDL_INDEX:
            mkgeom->num_indices = read_ints(in, &mkgeom->indices);
            break;

        case MDL_JOINT_IDS:
            mkgeom->num_joint_ids = read_ints(in, &mkgeom->joint_ids);
            break;

        case MDL_JOINT_WEIGHTS:
            mkgeom->num_weights = read_floats(in, &mkgeom->joint_weights);
            break;

        case MDL_TEX_COORD:
            read_floats(in, &mkgeom->tex_coords);
            break;
        case MDL_MAX:           read_nfloats(in, 3, lgeom->max); break;
        case MDL_MIN:           read_nfloats(in, 3, lgeom->min); break;
        case N_MDL_TAGS: assert(!"this shouldn't happend"); break;
        }
    }
}
