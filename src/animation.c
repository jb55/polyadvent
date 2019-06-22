
#include <stdio.h>
#include "xml.h"

struct dae_data {
    int opened;
    FILE *dae_file;
};

static void dae_tag_start(struct xmlparser *x, const char *t, size_t tl)
{
    struct dae_data *data = (struct dae_data*)x->user_data;

    for (int i = 0; i < data->opened; i++) {
        putchar(' ');
    }
    data->opened++;
    printf("%s\n", t);
}

static void dae_tag_end(struct xmlparser *x, const char *t, size_t tl, int what)
{
    struct dae_data *data = (struct dae_data*)x->user_data;

    data->opened--;
}

static int dae_getc(struct xmlparser *x)
{
    struct dae_data *data = (struct dae_data*)x->user_data;
    return fgetc(data->dae_file);
}

void collada_test(const char *filename)
{
    struct xmlparser x = {0};
    struct dae_data data = { .opened = 0 };

    data.dae_file = fopen("data/models/pirate-officer.dae", "rb");
    if (data.dae_file == NULL)
        exit(1);

    x.user_data = &data;
    x.xmltagstart = dae_tag_start;
    x.xmltagend   = dae_tag_end;
    x.getnext     = dae_getc;

    xml_parse(&x);

    fclose(data.dae_file);
}
