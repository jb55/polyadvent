
#include "resource.h"
#include "entity.h"
#include "model.h"
#include "util.h"
#include "procmesh.h"
#include "debug.h"
#include <assert.h>

static void print_int_resources(struct resource_manager *m)
{
    int *ints = (int*)m->resources;

    for (u32 i = 0; i < m->resource_count; i++) {
        printf("%d ", ints[i]);
    }

    printf("\n");
}


static void print_ids(struct resource_manager *m)
{
    for (u32 i = 0; i < m->resource_count; i++) {
        print_id(&m->ids[i], 0);
        printf(" ");
    }
    printf("\n");
}


static void test_compact()
{
    printf("test_compact\n");
    struct resource_manager r;
    struct resource_id ids[20];
    int *p;
    u32 i;

    init_resource_manager(&r, sizeof(int), 2, 6, "int", 0);

    for (i = 0; i < (int)ARRAY_SIZE(ids); i++)
        init_id(&ids[i]);

    i = 0;

    p = new_resource(&r, &ids[i++]);
    assert(r.resource_count == 1); *p = 0;

    p = new_resource(&r, &ids[i++]); *p = 1;
    assert(r.resource_count == 2);

    p = new_resource(&r, &ids[i++]); *p = 2;
    p = new_resource(&r, &ids[i++]);
    *p = 3;

    assert(r.resource_count == 4);
    destroy_resource(&r, &ids[1]);
    assert(r.resource_count == 3);

    assert(*(int*)get_resource(&r, &ids[0]) == 0);
    assert(       get_resource(&r, &ids[1]) == NULL);
    assert(*(int*)get_resource(&r, &ids[2]) == 2);

    new_resource(&r, &ids[i++]);
    assert(r.resource_count == 4);
    assert(r.current_capacity >= 4);

    new_resource(&r, &ids[i++]);
    assert(r.resource_count == 5);
    assert(r.current_capacity >= 5);
}

static void test_int_resource_manager()
{
    printf("test_int_resource_manager\n");
    struct resource_manager r;
    struct resource_id id, first_id;
    int *p;
    // 2 item case
    init_resource_manager(&r, sizeof(int), 1, 2, "int", 0);

    init_id(&id);
    init_id(&first_id);

    p = new_resource(&r, &first_id);
    assert(p);
    *p = 42;

    assert(r.resource_count == 1);
    assert(first_id.generation == 1);
    assert(*(int*)get_resource(&r, &first_id) == 42);

    p = new_resource(&r, &id);
    assert(p);
    *p = 32;
    assert(r.resource_count == 2);
    /* assert(id.generation == 2); */
    assert(*(int*)get_resource(&r, &id) == 32);
    assert(*(int*)get_resource(&r, &first_id) == 42);

    destroy_resource_manager(&r);
}

static void test_entity_system()
{
    printf("test_entity_system\n");
    u32 count;
    struct entity *ent, *ents;
    entity_id ent_id;
    entity_id *ids;

    init_node_manager();
    init_entity_system();

    init_id(&ent_id);

    ents = get_all_entities(&count, &ids);
    assert(count == RESERVED_ENTITIES);

    ent = new_entity(&ent_id);
    ents = get_all_entities(&count, &ids);

    assert(ent != NULL);
    assert(count == 1+RESERVED_ENTITIES);
    assert(&ents[RESERVED_ENTITIES] == ent);

    assert(ideq(&ids[RESERVED_ENTITIES], &ent_id));

    destroy_entity_system();
}

void test_geometry()
{
    printf("test_geometry\n");
    init_geometry_manager();
    init_model_manager();

    struct model *model;
    model_id player_model_id =
        get_static_model(model_pirate_officer, &model);

    struct geometry *geom = get_geometry(&model->geom_id);
    assert(geom);

    debug("pirate verts %d\n", geom->num_verts);
    /* assert(geom->num_verts == 2676); */

    model_id rock_model;
    init_id(&rock_model);
    struct model *pmodel  = new_model(&rock_model); assert(pmodel);
    struct geometry *pgeom = get_geometry(&pmodel->geom_id); assert(geom);
    proc_sphere(pgeom);

    model_id player_model_id2 = make_static_id(model_pirate_officer);

    assert(ideq(&player_model_id, &player_model_id2));

    pmodel = get_model(&player_model_id2);
    pgeom = get_geometry(&pmodel->geom_id);

    /* assert(geom->num_verts == 2676); */
}

void test_dynamic_model_manager()
{
    printf("test_dynamic_model_manager\n");
    init_model_manager();
}

int main(int argc, char *argv[])
{
    test_geometry();
    test_int_resource_manager();
    test_compact();
    test_dynamic_model_manager();
    test_entity_system();

    return 0;
}
