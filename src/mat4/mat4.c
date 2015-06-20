
#include "mat4.h"
#include <math.h>
#include <string.h>

mat4 *mat4_copy(const mat4 *src, mat4 *dst) {
  memcpy(dst, src, sizeof(float) * 16);
  return dst;
}


mat4 *mat4_id(mat4 *dst) {
  dst[0]  = 1.; dst[1]  = 0.; dst[2]  = 0.; dst[3]  = 0.;
  dst[4]  = 0.; dst[5]  = 1.; dst[6]  = 0.; dst[7]  = 0.;
  dst[8]  = 0.; dst[9]  = 0.; dst[10] = 1.; dst[11] = 0.;
  dst[12] = 0.; dst[13] = 0.; dst[14] = 0.; dst[15] = 1.;

  return dst;
}


mat4 *mat4_multiply(const mat4 *a, const mat4 *b, mat4 *dst) {
	float a00 = a[0],  a01 = a[1],  a02 = a[2],  a03 = a[3];
	float a10 = a[4],  a11 = a[5],  a12 = a[6],  a13 = a[7];
	float a20 = a[8],  a21 = a[9],  a22 = a[10], a23 = a[11];
	float a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];

	float b00 = b[0],  b01 = b[1],  b02 = b[2],  b03 = b[3];
	float b10 = b[4],  b11 = b[5],  b12 = b[6],  b13 = b[7];
	float b20 = b[8],  b21 = b[9],  b22 = b[10], b23 = b[11];
	float b30 = b[12], b31 = b[13], b32 = b[14], b33 = b[15];

	dst[0]  = b00*a00 + b01*a10 + b02*a20 + b03*a30;
	dst[1]  = b00*a01 + b01*a11 + b02*a21 + b03*a31;
	dst[2]  = b00*a02 + b01*a12 + b02*a22 + b03*a32;
	dst[3]  = b00*a03 + b01*a13 + b02*a23 + b03*a33;
	dst[4]  = b10*a00 + b11*a10 + b12*a20 + b13*a30;
	dst[5]  = b10*a01 + b11*a11 + b12*a21 + b13*a31;
	dst[6]  = b10*a02 + b11*a12 + b12*a22 + b13*a32;
	dst[7]  = b10*a03 + b11*a13 + b12*a23 + b13*a33;
	dst[8]  = b20*a00 + b21*a10 + b22*a20 + b23*a30;
	dst[9]  = b20*a01 + b21*a11 + b22*a21 + b23*a31;
	dst[10] = b20*a02 + b21*a12 + b22*a22 + b23*a32;
	dst[11] = b20*a03 + b21*a13 + b22*a23 + b23*a33;
	dst[12] = b30*a00 + b31*a10 + b32*a20 + b33*a30;
	dst[13] = b30*a01 + b31*a11 + b32*a21 + b33*a31;
	dst[14] = b30*a02 + b31*a12 + b32*a22 + b33*a32;
	dst[15] = b30*a03 + b31*a13 + b32*a23 + b33*a33;

  return dst;
}

// TODO: util me
int float_eq(float a, float b);
int float_eq(float a, float b) {
  return fabsf(a - b) < 0.0001;
}

mat4 *mat4_rotate(const mat4 *mat, const float angle,
                  const float *axis, mat4 *dest) {

	float x = axis[0], y = axis[1], z = axis[2];
	float len = (float)sqrt(x*x + y*y + z*z);

	if (float_eq(len, 0.)) { return NULL; }
  // TODO: float comparison tool
	if (!float_eq(len, 1.)) {
		len = 1 / len;
		x *= len;
		y *= len;
		z *= len;
	}

	float s = (float)sin(angle);
	float c = (float)cos(angle);
	float t = 1-c;

	// Cache the matrix values (makes for huge speed increases!)
	float a00 = mat[0], a01 = mat[1], a02 = mat[2], a03  = mat[3];
	float a10 = mat[4], a11 = mat[5], a12 = mat[6], a13  = mat[7];
	float a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11];

	// Construct the elements of the rotation matrix
	float b00 = x*x*t + c, b01 = y*x*t + z*s, b02 = z*x*t - y*s;
	float b10 = x*y*t - z*s, b11 = y*y*t + c, b12 = z*y*t + x*s;
	float b20 = x*z*t + y*s, b21 = y*z*t - x*s, b22 = z*z*t + c;

  // If the source and destination differ, copy the unchanged last row
	if(mat != dest) { 
		dest[12] = mat[12];
		dest[13] = mat[13];
		dest[14] = mat[14];
		dest[15] = mat[15];
	}

	// Perform rotation-specific matrix multiplication
	dest[0] = a00*b00 + a10*b01 + a20*b02;
	dest[1] = a01*b00 + a11*b01 + a21*b02;
	dest[2] = a02*b00 + a12*b01 + a22*b02;
	dest[3] = a03*b00 + a13*b01 + a23*b02;

	dest[4] = a00*b10 + a10*b11 + a20*b12;
	dest[5] = a01*b10 + a11*b11 + a21*b12;
	dest[6] = a02*b10 + a12*b11 + a22*b12;
	dest[7] = a03*b10 + a13*b11 + a23*b12;

	dest[8] = a00*b20 + a10*b21 + a20*b22;
	dest[9] = a01*b20 + a11*b21 + a21*b22;
	dest[10] = a02*b20 + a12*b21 + a22*b22;
	dest[11] = a03*b20 + a13*b21 + a23*b22;

	return dest;
}

