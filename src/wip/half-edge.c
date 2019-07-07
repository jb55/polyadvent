

#include "half-edge.h"

// NOTE: end = start(next(e))

inline u32 he_start(struct half_edge *he)
{
    return he->start;
}

inline struct half_edge *he_next(struct half_edge *edges, struct half_edge *he)
{
    return &edges[he->next];
}

inline u32 he_end(struct half_edge *edges, struct half_edge *he)
{
    return he_start(he_next(edges, he));
}

inline u32 he_prev_vert(struct half_edge *he)
{
}
