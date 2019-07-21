
#ifndef STATIC_RESOURCES_H
#define STATIC_RESOURCES_H

enum static_model {
    model_cube,
    model_tower,
    model_icosphere,
    model_pirate_officer,
    model_barrel,
    NUM_STATIC_MODELS
};

enum static_entities {
    entity_terrain,
    entity_player,
    RESERVED_ENTITIES
};

// this should align with static entities
enum static_nodes {
    node_terrain,
    node_player,
    N_STATIC_NODES
};


#endif /* STATIC_RESOURCES_H */
