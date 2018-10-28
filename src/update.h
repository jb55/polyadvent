
#ifndef PA_UPDATE_H
#define PA_UPDATE_H

#include "game.h"

void update(struct game * game, u32 dt);
void resize_fbos(struct game * game, int width, int height);

#endif /* PA_UPDATE_H */
