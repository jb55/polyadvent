

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

static void delete_every_other_entity()
{
    u32 count;
    entity_id *ids;

    for (u32 i = RESERVED_ENTITIES; i < 1000; i++) {
        get_all_entities(&count, &ids);

        if (i >= count)
            return;

        if (i % 2 == 0) {
            struct entity *ent = get_entity(&ids[i]); assert(ent);
            struct model *pmodel = get_model(&ent->model_id); assert(pmodel);

            destroy_model(&ent->model_id);
            destroy_entity(&ids[i]);
        }
    }
}



void entity_test_scene(struct game *game)
{
    struct entity *player  = get_player(&game->test_resources);
    struct terrain *terrain  = &game->terrain;
    player->model_id = get_static_model(model_pirate_officer, NULL);
    model_id proc_id;
    init_model_id(&proc_id);

    struct model *pmodel  = new_model(&proc_id); assert(pmodel);
    struct geometry *geom = get_geometry(&pmodel->geom_id); assert(geom);
    proc_sphere(geom);
    pmodel->shading = SHADING_VERT_COLOR;

    for (int i = 0; i < 500; i++) {
        struct entity *ent = new_entity(NULL);
        struct node *node  = get_node(&ent->node_id);

        ent->model_id = proc_id;

        double x = rand_0to1() * terrain->size;
        double y = rand_0to1() * terrain->size;
        double z = terrain->fn(terrain, x, y);

        node_scale(node, 10.0);
        node_rotate(node, V3(rand_0to1(),rand_0to1(),rand_0to1()));
        node_translate(node, V3(x, y, z));

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
