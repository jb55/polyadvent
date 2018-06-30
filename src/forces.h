
#ifndef POLYADVENT_FORCES_H
#define POLYADVENT_FORCES_H

#include "entity.h"

void force_apply(struct entity *entity, float *force);

void impulse_apply(struct entity *entity, float *impulse);


#endif /* POLYADVENT_FORCES_H */
