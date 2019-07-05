

#include "game.h"
#include "model.h"
#include "procmesh.h"
#include "debug.h"
#include "vec3.h"
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



void entity_test_scene(struct game *game)
{
    struct entity *player  = get_player(&game->test_resources);
    struct terrain *terrain  = &game->terrain;

    model_id rock_model;
    init_id(&rock_model);

    /* model_id rock_model = get_static_model(model_tower, NULL); */
    struct model *pmodel  = new_model(&rock_model); assert(pmodel);
    struct geometry *geom = get_geometry(&pmodel->geom_id); assert(geom);
    printf("rock geom ");
    print_id(&pmodel->geom_id, 1);
    printf("rock model ");
    print_id(&rock_model, 1);
    proc_sphere(geom);

    for (int i = 0; i < terrain->size*0.2; i++) {
        struct entity *ent = new_entity(NULL);
        struct node *node  = get_node(&ent->node_id);

        ent->model_id = rock_model;

        double x = rand_0to1() * terrain->size;
        double y = rand_0to1() * terrain->size;
        double z = terrain->fn(terrain, x, y);

        node_scale(node, pow(15.0, rand_0to1()));
        node_rotate(node, V3(rand_0to1(),rand_0to1(),rand_0to1()));
        node_translate(node, V3(x, y, z));
        node_set_label(node, "rock");

        node_recalc(node);
    }

}

void pbr_scene(struct game *game)
{
    struct entity *ent = new_entity(NULL);
    struct node *node  = get_node(&ent->node_id); assert(node);
    struct entity *player = get_player(&game->test_resources);

    ent->model_id = get_static_model(model_icosphere, NULL);
    node_set_label(node, "sphere");
}
