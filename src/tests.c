
#include "scene.h"
#include "entity.h"


/* #define t_assert(cond, msg) if (cond) { printf(msg); } */
#define t_assert(cond, msg) assert(cond);

int scene_tests(struct game *game) {
    struct node *root = &game->test_resources.root;
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
    t_assert(ent_count == RESERVED_ENTITIES,
           "scene: entity count isn't reset after reset_scene");

    t_assert(node_count(root) == initial_node_count,
           "scene: node count doesn't match initial after reset_scene");

    return 1;
}
