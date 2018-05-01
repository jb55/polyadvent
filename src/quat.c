
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

void quat_to_mat3(quat *quat, float *dest) {
	float x = quat[0], y = quat[1], z = quat[2], w = quat[3];

	float x2 = x + x;
	float y2 = y + y;
	float z2 = z + z;

	float xx = x*x2;
	float xy = x*y2;
	float xz = x*z2;

	float yy = y*y2;
	float yz = y*z2;
	float zz = z*z2;

	float wx = w*x2;
	float wy = w*y2;
	float wz = w*z2;

	dest[0] = 1.0 - (yy + zz);
	dest[1] = xy - wz;
	dest[2] = xz + wy;

	dest[3] = xy + wz;
	dest[4] = 1.0 - (xx + zz);
	dest[5] = yz - wx;

	dest[6] = xz - wy;
	dest[7] = yz + wx;
	dest[8] = 1.0 - (xx + yy);
}

void quat_multiply_vec3(quat *quat, float *vec, float *dest) {
	float x  = vec[0], y = vec[1], z = vec[2];
	float qx = quat[0], qy = quat[1], qz = quat[2], qw = quat[3];

	// calculate quat * vec
	float ix = qw*x + qy*z - qz*y;
	float iy = qw*y + qz*x - qx*z;
	float iz = qw*z + qx*y - qy*x;
	float iw = -qx*x - qy*y - qz*z;

	// calculate result * inverse quat
	dest[0] = ix*qw + iw*-qx + iy*-qz - iz*-qy;
	dest[1] = iy*qw + iw*-qy + iz*-qx - ix*-qz;
	dest[2] = iz*qw + iw*-qz + ix*-qy - iy*-qx;
}

quat *quat_inverse(quat *q, quat *dest) {
	if(!dest || q == dest) {
		q[0] *= 1;
		q[1] *= 1;
		q[2] *= 1;
		return q;
	}
	dest[0] = -q[0];
	dest[1] = -q[1];
	dest[2] = -q[2];
	dest[3] = q[3];
	return dest;
}
