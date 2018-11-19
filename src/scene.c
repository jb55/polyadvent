
#include "game.h"
#include "model.h"
#include <assert.h>

void reset_scene(struct game *game) {
    struct entity *terrain_ent = get_terrain_entity(&game->terrain);
    struct entity *player_ent = get_player(&game->test_resources);

    // hide terrain and player by default
    terrain_ent->flags |= ENT_INVISIBLE;
    player_ent->flags |= ENT_INVISIBLE;
}

void default_scene(struct game *game) {
    struct terrain *terrain = &game->terrain;
    struct entity *terrain_ent = get_terrain_entity(terrain);
    struct entity *player = get_player(&game->test_resources);

    // show terrain
    terrain_ent->flags &= ~ENT_INVISIBLE;

    // show player
    player->flags &= ~ENT_INVISIBLE;

    struct entity *tower = new_entity(NULL);
    int ok = load_model(&tower->model, "tower");
    assert(ok);
    tower->node.label = "tower";
    node_attach(&tower->node, &player->node);
    node_translate(&tower->node, V3(0.0, 50.0, 0.0));
    node_recalc(&tower->node);
    float z = terrain->fn(terrain, tower->node.mat[M_X], tower->node.mat[M_Y]);
    node_detach(&tower->node, &player->node);
    tower->node.mat[M_Z] = z;
    // END ENTITIES


}

void pbr_scene(struct game *game) {

}
