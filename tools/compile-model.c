
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "dae.h"
#include "debug.h"
#include "mdl.h"


void usage()
{
    printf("usage: compile-model <model.dae> <out.mdl>\n");
    exit(1);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        usage();

    init_node_manager();
    init_geometry_manager();
    init_model_manager();

    const char *filename = argv[1];
    const char *outfile = argv[2];

    struct model model;
    struct mdl_geometry mdl_geom;

    init_model(&model);
    init_mdl_geometry(&mdl_geom);

    load_dae(filename, &model, &mdl_geom.mkgeom);

    save_mdl(outfile, &model, &mdl_geom);

    return 0;
}
