
#ifndef ENTITY_H
#define ENTITY_H

#include "node.h"
#include "model.h"

struct entity {
    struct node node;
    struct model model;
    float velocity[3];
    float accel[3];
    int casts_shadows;
};

struct entity_id;

struct entity_system {
    struct entity *entities;
    struct entity_id *handles;
    int entity_count;
    u32 generation;
};

void init_entity(struct entity *);
void init_entity_system(struct entity_system *);
struct entity *get_entity(struct entity_system *, struct entity_id);


#endif /* ENTITY_H */
