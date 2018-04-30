
#include "quat.h"
#include <math.h>

void quat_id(quat *dest) {
  dest[0] = 0;
  dest[1] = 0;
  dest[2] = 0;
  dest[3] = 1;
}

void quat_multiply(quat *a, quat *b, quat *dest) {
  float ax = a[0];
  float ay = a[1];
  float az = a[2];
  float aw = a[3];

  float bx = b[0];
  float by = b[1];
  float bz = b[2];
  float bw = b[3];

  dest[0] = ax * bw + aw * bx + ay * bz - az * by;
  dest[1] = ay * bw + aw * by + az * bx - ax * bz;
  dest[2] = az * bw + aw * bz + ax * by - ay * bx;
  dest[3] = aw * bw - ax * bx - ay * by - az * bz;
}


void quat_axis_angle(float *axis, float angle, quat *dest) {
  float half_angle = angle / 2.0;
  float s = sin(half_angle);

  dest[0] = axis[0] * s;
  dest[1] = axis[1] * s;
  dest[2] = axis[2] * s;
  dest[3] = cos(half_angle);
}
