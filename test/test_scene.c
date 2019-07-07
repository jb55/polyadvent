
#include "scene.h"
#include "entity.h"
#include "game.h"
#include "debug.h"
#include <assert.h>

#include <unistd.h>


/* #define t_assert(cond, msg) if (cond) { printf(msg); } */
#define t_assert(cond, msg) assert(cond && msg);


void delete_every_other_entity()
{
    u32 count;
    entity_id *ids;

    for (u32 i = RESERVED_ENTITIES; i < 1000; i++) {
        get_all_entities(&count, &ids);

        if (i >= count)
            return;

        if (i % 2 == 0) {
            struct entity *ent = get_entity(&ids[i]); assert(ent);
            struct model *pmodel = get_model(&ent->model_id);

            if (pmodel)
                destroy_model(&ent->model_id);
            destroy_entity(&ids[i]);
        }
    }
}


int scene_tests(struct game *game) {
    struct node *root = get_node(&game->test_resources.root_id);
    assert(root);

    u32 ent_count;
    int initial_node_count = node_count(root);
    reset_scene(game);
    t_assert(node_count(root) == initial_node_count,
           "scene: node count doesn't match initial after reset_scene");

    get_all_entities(&ent_count, NULL);
    t_assert(ent_count == RESERVED_ENTITIES,
           "scene: entity count isn't reset after reset_scene");


    default_scene(game);
    get_all_entities(&ent_count, NULL);

    t_assert(ent_count > RESERVED_ENTITIES,
           "scene: entity count isn't larger after loading default scene");

    reset_scene(game);
    get_all_entities(&ent_count, NULL);
    t_assert(ent_count == RESERVED_ENTITIES,
           "scene: entity count isn't reset after reset_scene");

    t_assert(node_count(root) == initial_node_count,
           "scene: node count doesn't match initial after reset_scene");

    entity_test_scene(game);

    get_all_entities(&ent_count, NULL);
    /* assert(ent_count == 102); */

    delete_every_other_entity();

    return 1;
}


int main(int argc, char *argv[])
{
    struct game game;
    int res = chdir("/home/jb55/src/c/polyadvent");
    assert(res == 0);
    game_init(&game, 1024, 768);
    scene_tests(&game);
    return 0;
}
