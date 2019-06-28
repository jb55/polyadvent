
#include "resource.h"
#include "entity.h"
#include "model.h"
#include "debug.h"
#include <assert.h>

void test_compact()
{
    struct resource_manager r;
    struct resource_id id, first_id;
    int *p;

    init_resource_manager(&r, sizeof(int), 2, 4);
    p = new_resource(&r, &first_id);
    *p = 11;
    p = new_resource(&r, &id);
    *p = 22;
    assert(r.current_capacity == 2);
    destroy_resource(&r, &first_id);
    assert(r.resource_count == 2);
    new_resource(&r, &id);
    assert(r.resource_count == 2);
    assert(r.current_capacity == 2);
    new_resource(&r, &id);
    assert(r.current_capacity == 4);
}

void test_int_resource_manager()
{
    struct resource_manager r;
    struct resource_id id, first_id;
    int *p;
    // 2 item case
    init_resource_manager(&r, sizeof(int), 1, 2);

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
    u32 count;
    struct entity *ent, *ents;
    entity_id ent_id;
    entity_id *ids;

    init_entity_system();

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
    init_model_manager();
}

int main(int argc, char *argv[])
{
    /* test_int_resource_manager(); */
    test_compact();
    /* test_dynamic_model_manager(); */
    test_entity_system();

    return 0;
}
