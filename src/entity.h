
#ifndef ENTITY_H
#define ENTITY_H

#include "node.h"
#include "model.h"

struct entity {
    struct node node;
    struct model model;
    int casts_shadows;
};

void init_entity(struct entity *);


#endif /* ENTITY_H */
