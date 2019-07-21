
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "debug.h"
#include "mdl.h"
#include "ply.h"


void usage()
{
    printf("usage: compile-model <model.dae> <out.mdl>\n");
    exit(1);
}

// ext len must be 3
static const char *replace_ext(char *filename, int len, const char *ext)
{
    int replaced = 0;
    bool replacing = false;
    for (int i = 0; i < len; i++) {
        if (replacing) {
            if (replaced == 3) {
                filename[replaced] = 0;
                return filename;
            }
            filename[i] = ext[replaced++];
        }
        else if (filename[i] == '.')  {
            replacing = true;
        }
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        usage();

    init_node_manager();
    init_geometry_manager();
    init_model_manager();

    char *filename = argv[1];
    const char *outfile = argv[2];

    struct model model;
    struct mdl_geometry mdl_geom;
    struct make_geometry *mkgeom = &mdl_geom.mkgeom;

    init_model(&model);
    init_mdl_geometry(&mdl_geom);

    /* load_dae(filename, &model, NULL); */
    /* replace_ext(filename, strlen(filename), "dae"); */

    int ok = parse_ply_with_mkgeom(filename, &mdl_geom);
    assert(ok);

    save_mdl(outfile, &model, &mdl_geom);

    return 0;
}
