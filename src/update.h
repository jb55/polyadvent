
#ifndef PA_UPDATE_H
#define PA_UPDATE_H

#include "game.h"

void update(struct game * game);
void resize_fbos(struct entity *player, struct fbo *shadow_buffer,
                 float *mat4_ortho, int width, int height);

#endif /* PA_UPDATE_H */
