
#include <stdio.h>
#include "xml.h"
#include "util.h"

enum dae_state {
    PARSING_START,
    PARSING_NODE,
};

struct dae_data {
    int opened;
    int state;
    FILE *dae_file;
};

static void dae_tag_start(struct xmlparser *x, const char *t, size_t tl)
{
    struct dae_data *data = (struct dae_data*)x->user_data;
    data->opened++;

    if (streq(t, "node"))
        data->state = PARSING_NODE;

    if (data->state != PARSING_NODE)
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

static int dae_getc(struct xmlparser *x)
{
    struct dae_data *data = (struct dae_data*)x->user_data;
    return fgetc(data->dae_file);
}

void dae_attr(struct xmlparser *x, const char *t, size_t tl,
              const char *a, size_t al, const char *v, size_t vl)
{
    struct dae_data *data = (struct dae_data*)x->user_data;
    if (data->state == PARSING_NODE &&
        streq(a, "id") &&
        memeq(v, "Armature", sizeof("Armature")-1, vl))
    {
        printf(" %s=%s", a, v);
    }
}

void collada_test(const char *filename)
{
    struct xmlparser x = {0};
    struct dae_data data = {
      .opened = 0,
      .state  = PARSING_START,
    };

    data.dae_file = fopen("data/models/pirate-officer.dae", "rb");
    if (data.dae_file == NULL)
        exit(1);

    x.user_data = &data;
    x.xmltagstart = dae_tag_start;
    x.xmltagend   = dae_tag_end;
    x.xmlattr     = dae_attr;
    /* x.xmlattrend  = dae_attr_end; */
    x.getnext     = dae_getc;

    xml_parse(&x);

    fclose(data.dae_file);
}
