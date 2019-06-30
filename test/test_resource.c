
#include "resource.h"
#include "entity.h"
#include "model.h"
#include "util.h"
#include "debug.h"
#include <assert.h>

void test_compact()
{
    printf("test_compact\n");
    struct resource_manager r;
    struct resource_id ids[3], first_id;
    int *p;

    init_resource_manager(&r, sizeof(int), 2, 4);

    for (int i = 0; i < (int)ARRAY_SIZE(ids); i++)
        init_id(&ids[i]);

    init_id(&first_id);

    p = new_resource(&r, &first_id);
    assert(r.resource_count == 1);
    *p = 11;

    p = new_resource(&r, &ids[0]);
    *p = 22;
    assert(r.resource_count == 2);

    destroy_resource(&r, &first_id);
    assert(r.resource_count == 1);
    assert(get_resource(&r, &first_id) == NULL);
    assert(*(int*)get_resource(&r, &ids[0]) == 22);

    new_resource(&r, &ids[1]);
    assert(r.resource_count == 2);
    assert(r.current_capacity == 2);

    new_resource(&r, &ids[2]);
    assert(r.resource_count == 3);
    assert(r.current_capacity >= 3);
}

void test_int_resource_manager()
{
    printf("test_int_resource_manager\n");
    struct resource_manager r
;
    struct resource_id id, first_id;
    int *p;
    // 2 item case
    init_resource_manager(&r, sizeof(int), 1, 2);

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

void test_entity_system()
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
    assert(count == 0);

    ent = new_entity(&ent_id);
    ents = get_all_entities(&count, &ids);

    assert(ent != NULL);
    assert(count == 1);
    assert(&ents[0] == ent);

    assert(ideq(&ids[0], &ent_id));

    destroy_entity_system();
}

void test_dynamic_model_manager()
{
    printf("test_dynamic_model_manager\n");
    init_model_manager();
}

int main(int argc, char *argv[])
{
    test_int_resource_manager();
    test_compact();
    test_dynamic_model_manager();
    test_entity_system();

    return 0;
}
