
#ifndef POLYADVENT_CAMERA_H
#define POLYADVENT_CAMERA_H

#include "mat4.h"
#include "vec3.h"
#include "node.h"

struct camera {
  float frustum[16];
  struct node node;
};

void
camera_follow(vec3 *cam_pos, vec3 *target_prev, vec3 *target, mat4 *cam);

#endif /* POLYADVENT_CAMERA_H */
