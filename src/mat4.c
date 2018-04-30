
#include "mat4.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include "vec3.h"

#define PI 3.14159265f

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

mat4 *mat4_transpose(mat4 *src, mat4 *dest) {
	// If we are transposing ourselves we can skip a few steps but have to cache some values
	if(dest == NULL || src == dest) {
		float a01 = src[1], a02 = src[2], a03 = src[3];
		float a12 = src[6], a13 = src[7];
		float a23 = src[11];

		src[1]  = src[4];
		src[2]  = src[8];
		src[3]  = src[12];
		src[4]  = a01;
		src[6]  = src[9];
		src[7]  = src[13];
		src[8]  = a02;
		src[9]  = a12;
		src[11] = src[14];
		src[12] = a03;
		src[13] = a13;
		src[14] = a23;
		return src;
	}

	dest[0]  = src[0];
	dest[1]  = src[4];
	dest[2]  = src[8];
	dest[3]  = src[12];
	dest[4]  = src[1];
	dest[5]  = src[5];
	dest[6]  = src[9];
	dest[7]  = src[13];
	dest[8]  = src[2];
	dest[9]  = src[6];
	dest[10] = src[10];
	dest[11] = src[14];
	dest[12] = src[3];
	dest[13] = src[7];
	dest[14] = src[11];
	dest[15] = src[15];
	return dest;

}

/*
 * mat4.frustum
 * Generates a frustum matrix with the given bounds
 *
 * Params:
 * left, right - scalar, left and right bounds of the frustum
 * bottom, top - scalar, bottom and top bounds of the frustum
 * near, far - scalar, near and far bounds of the frustum
 * dest - Optional, mat4 frustum matrix will be written into
 *
 * Returns:
 * dest if specified, a new mat4 otherwise
 */
mat4 *mat4_frustum (float left, float right, float bottom,
                    float top, float near, float far, mat4 *dest) {
	float rl = (right - left);
	float tb = (top - bottom);
	float fn = (far - near);
	dest[0] = (near*2) / rl;
	dest[1] = 0;
	dest[2] = 0;
	dest[3] = 0;
	dest[4] = 0;
	dest[5] = (near*2) / tb;
	dest[6] = 0;
	dest[7] = 0;
	dest[8] = (right + left) / rl;
	dest[9] = (top + bottom) / tb;
	dest[10] = -(far + near) / fn;
	dest[11] = -1;
	dest[12] = 0;
	dest[13] = 0;
	dest[14] = -(far*near*2) / fn;
	dest[15] = 0;
	return dest;
}

/*
 * mat4.translate
 * Translates a matrix by the given vector
 *
 * Params:
 * mat - mat4 to translate
 * vec - vec3 specifying the translation
 * dest - Optional, mat4 receiving operation result. If not specified result is written to mat
 *
 * Returns:
 * dest if specified, mat otherwise
 */
mat4 *mat4_translate (mat4 *mat, float *v3, mat4 *dest) {
  const float x = v3[0];
  const float y = v3[1];
  const float z = v3[2];

	if(!dest || mat == dest) {
		mat[12] = mat[0]*x + mat[4]*y + mat[8]*z + mat[12];
		mat[13] = mat[1]*x + mat[5]*y + mat[9]*z + mat[13];
		mat[14] = mat[2]*x + mat[6]*y + mat[10]*z + mat[14];
		mat[15] = mat[3]*x + mat[7]*y + mat[11]*z + mat[15];
		return mat;
	}

	float a00 = mat[0], a01 = mat[1], a02 = mat[2], a03 = mat[3];
	float a10 = mat[4], a11 = mat[5], a12 = mat[6], a13 = mat[7];
	float a20 = mat[8], a21 = mat[9], a22 = mat[10], a23 = mat[11];

	dest[0] = a00;
	dest[1] = a01;
	dest[2] = a02;
	dest[3] = a03;
	dest[4] = a10;
	dest[5] = a11;
	dest[6] = a12;
	dest[7] = a13;
	dest[8] = a20;
	dest[9] = a21;
	dest[10] = a22;
	dest[11] = a23;

	dest[12] = a00*x + a10*y + a20*z + mat[12];
	dest[13] = a01*x + a11*y + a21*z + mat[13];
	dest[14] = a02*x + a12*y + a22*z + mat[14];
	dest[15] = a03*x + a13*y + a23*z + mat[15];
	return dest;
}

mat4 *mat4_perspective(float fov, float aspect, float near,
                       float far, mat4 *dest) {
	float top = near * tanf(fov*PI / 360.0f);
	float right = top * aspect;
	return mat4_frustum(-right, right, -top, top, near, far, dest);
}

mat4 *mat4_inverse(mat4 *src, mat4 *dest) {
	if(dest == NULL) { dest = src; }

	// Cache the srcrix values (makes for huge speed increases!)
	float a00 = src[0], a01  = src[1], a02  = src[2], a03  = src[3];
	float a10 = src[4], a11  = src[5], a12  = src[6], a13  = src[7];
	float a20 = src[8], a21  = src[9], a22  = src[10], a23 = src[11];
	float a30 = src[12], a31 = src[13], a32 = src[14], a33 = src[15];

	float b00 = a00*a11 - a01*a10;
	float b01 = a00*a12 - a02*a10;
	float b02 = a00*a13 - a03*a10;
	float b03 = a01*a12 - a02*a11;
	float b04 = a01*a13 - a03*a11;
	float b05 = a02*a13 - a03*a12;
	float b06 = a20*a31 - a21*a30;
	float b07 = a20*a32 - a22*a30;
	float b08 = a20*a33 - a23*a30;
	float b09 = a21*a32 - a22*a31;
	float b10 = a21*a33 - a23*a31;
	float b11 = a22*a33 - a23*a32;

	// Calculate the determinant (inlined to avoid double-caching)
	float invDet = 1/(b00*b11 - b01*b10 + b02*b09 + b03*b08 - b04*b07 + b05*b06);

	dest[0] = (a11*b11 - a12*b10 + a13*b09)*invDet;
	dest[1] = (-a01*b11 + a02*b10 - a03*b09)*invDet;
	dest[2] = (a31*b05 - a32*b04 + a33*b03)*invDet;
	dest[3] = (-a21*b05 + a22*b04 - a23*b03)*invDet;
	dest[4] = (-a10*b11 + a12*b08 - a13*b07)*invDet;
	dest[5] = (a00*b11 - a02*b08 + a03*b07)*invDet;
	dest[6] = (-a30*b05 + a32*b02 - a33*b01)*invDet;
	dest[7] = (a20*b05 - a22*b02 + a23*b01)*invDet;
	dest[8] = (a10*b10 - a11*b08 + a13*b06)*invDet;
	dest[9] = (-a00*b10 + a01*b08 - a03*b06)*invDet;
	dest[10] = (a30*b04 - a31*b02 + a33*b00)*invDet;
	dest[11] = (-a20*b04 + a21*b02 - a23*b00)*invDet;
	dest[12] = (-a10*b09 + a11*b07 - a12*b06)*invDet;
	dest[13] = (a00*b09 - a01*b07 + a02*b06)*invDet;
	dest[14] = (-a30*b03 + a31*b01 - a32*b00)*invDet;
	dest[15] = (a20*b03 - a21*b01 + a22*b00)*invDet;

	return dest;
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

mat4 *mat4_scale(mat4 *a, float v[3], mat4 *out) {
  float x = v[0], y = v[1], z = v[2];

  out[0] = a[0] * x;
  out[1] = a[1] * x;
  out[2] = a[2] * x;
  out[3] = a[3] * x;
  out[4] = a[4] * y;
  out[5] = a[5] * y;
  out[6] = a[6] * y;
  out[7] = a[7] * y;
  out[8] = a[8] * z;
  out[9] = a[9] * z;
  out[10] = a[10] * z;
  out[11] = a[11] * z;
  out[12] = a[12];
  out[13] = a[13];
  out[14] = a[14];
  out[15] = a[15];
  return out;
};


void mat4_print(const mat4 *m) {
  for (int i = 0; i < 16; ++i) {
    if (i % 4 == 0)
      printf("\n");
    printf("%f, ", m[i]);
  }
  printf("\n");
}

/* mat4 *mat4_create_transform(float *pos, float *scale, float *rot, mat4 *dest) { */
/*     if (!dest) { dest = mat4.create(); } */

/*     float halfAngle = 0.5*angle; */
/*     float sin_ = sin(halfAngle); */

/*     dest[0] = sin_*axis[0]; */
/*     dest[1] = sin_*axis[1]; */
/*     dest[2] = sin_*axis[2]; */
/*     dest[3] = cos(halfAngle); */

/*     var rot = quat4.toMat3(orientation); */

/*     dest[0] = scale[0] * rot[0]; */
/*     dest[1] = scale[0] * rot[1]; */
/*     dest[2] = scale[0] * rot[2]; */
/*     dest[3] = 0; */

/*     dest[4] = scale[1] * rot[3]; */
/*     dest[5] = scale[1] * rot[4]; */
/*     dest[6] = scale[1] * rot[5]; */
/*     dest[7] = 0; */

/*     dest[8] = scale[2] * rot[6]; */
/*     dest[9] = scale[2] * rot[7]; */
/*     dest[10] = scale[2] * rot[8]; */
/*     dest[11] = 0; */

/*     dest[12] = position[0]; */
/*     dest[13] = position[1]; */
/*     dest[14] = position[2]; */
/*     dest[15] = 1; */

/*     return dest; */
/*   } */
/* } */
