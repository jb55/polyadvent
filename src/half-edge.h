
#ifndef HALF_EDGE_H
#define HALF_EDGE_H



#include "common.h"

struct point

struct half_edge {
    u32 opposite;
    struct half_edge *next;
    struct half_edge *prev;
    u32 start;
};

struct face {
};




#endif /* HALF-EDGE_H */
