
#ifndef POLYADVENT_MAT_UTIL_H
#define POLYADVENT_MAT_UTIL_H

#include "vec3.h"
#include "mat4.h"
#include "quat.h"

void look_at(vec3 *eye, vec3 *target, vec3 *up, mat4 *dest);
mat4 *mat4_create_transform(float *pos, float *scale, quat *orientation, mat4 *dest);

#endif /* POLYADVENT_MAT_UTIL_H */
