
#include "mat_util.h"

mat4 *mat4_create_transform(float *pos, float *scale, float *rot, mat4 *dest) {
  dest[0] = scale[0] * rot[0];
  dest[1] = scale[0] * rot[1];
  dest[2] = scale[0] * rot[2];
  dest[3] = 0;

  dest[4] = scale[1] * rot[3];
  dest[5] = scale[1] * rot[4];
  dest[6] = scale[1] * rot[5];
  dest[7] = 0;

  dest[8] = scale[2] * rot[6];
  dest[9] = scale[2] * rot[7];
  dest[10] = scale[2] * rot[8];
  dest[11] = 0;

  dest[12] = pos[0];
  dest[13] = pos[1];
  dest[14] = pos[2];
  dest[15] = 1;

  return dest;
}


vec3 *vec3_forward(vec3 *v, quat *orientation, float *dir, float *dest) {
    float q[4] = {0};
    float movement[3] = {0};
    quat_inverse(orientation, q);
    quat_multiply_vec3(q, dir, movement);
    vec3_add(v, movement, dest);
    return dest;
}
