
#include "game.h"
#include "model.h"
#include <assert.h>

void reset_scene(struct game *game) {
    struct entity *terrain_ent = get_terrain_entity(&game->terrain);
    struct entity *player_ent = get_player(&game->test_resources);

    destroy_entities();

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
    struct node *tnode = get_node(&tower->node_id);
    struct node *pnode = get_node(&player->node_id);

    assert(tnode);
    tower->model_id = get_static_model(model_tower, NULL);
    node_set_label(tnode, "tower");
    node_attach(&tower->node_id, &player->node_id);
    node_translate(tnode, V3(0.0, 50.0, 0.0));
    node_recalc(tnode);
    float z = terrain->fn(terrain, tnode->mat[M_X], tnode->mat[M_Y]);
    /* node_detach(tnode, pnode); */
    tnode->mat[M_Z] = z;
    // END ENTITIES


}

void pbr_scene(struct game *game) {
    struct entity *ent = new_entity(NULL);
    struct node *node  = get_node(&ent->node_id); assert(node);

    ent->model_id = get_static_model(model_icosphere, NULL);
    node_set_label(node, "sphere");
    node_attach(&game->test_resources.root_id, &ent->node_id);

}
