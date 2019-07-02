
#include "scene.h"
#include "entity.h"
#include "game.h"
#include "debug.h"
#include <assert.h>

#include <unistd.h>


/* #define t_assert(cond, msg) if (cond) { printf(msg); } */
#define t_assert(cond, msg) assert(cond && msg);


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
