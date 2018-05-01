
#include "mat_util.h"

mat4 *mat4_create_transform(float *pos, float *scale, quat *orientation, mat4 *dest) {
  float rot[9];
  quat_to_mat3(orientation, rot);

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
