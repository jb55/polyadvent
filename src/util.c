
#include "util.h"
#include "vec3.h"
#include "mat4.h"
#include "quat.h"
#include <stdlib.h>

int clampi(int a, int mina, int maxa) {
  if (a > maxa)
    return maxa;

  if (a < mina)
    return mina;

  return a;
}

double max(double a, double b) {
  return a > b ? a : b;
}

double min(double a, double b) {
  return a < b ? a : b;
}

double clamp(double a, double mina, double maxa) {
  if (a > maxa)
    return maxa;

  if (a < mina)
    return mina;

  return a;
}

void check_gl() {
  unsigned int e = glGetError();
  if (e != GL_NO_ERROR) {
    printf("Got error from OpenGL: %d\n", e);
    assert(0);
  }
}


double rand_0to1() {
  return (double) rand() / RAND_MAX;
}

void look_at(vec3 *eye, vec3 *target, vec3 *up, mat4 *dest) {
  float z[3], x[3], y[3];

  vec3_subtract(eye, target, z);

  /* if (vec3_lengthsq(z) == 0) */
  /*   z[2] = 0; */

  vec3_normalize(z, z);
  vec3_cross(up, z, x);

  if (vec3_lengthsq(x) == 0) {
    // up and z are parallel

    if (fabs(up[2]) == 1.0)
      z[0] += 0.0001;
    else
      z[2] += 0.0001;

    vec3_normalize(z, z);
    vec3_cross(up, z, x);
  }

  vec3_normalize(x, x);
  vec3_cross(z, x, y);

  dest[0] = x[0];
  dest[1] = x[1];
  dest[2] = x[2];
  /* dest[3] = -vec3_dot(x, eye); */

  dest[4] = y[0];
  dest[5] = y[1];
  dest[6] = y[2];
  /* dest[7] = -vec3_dot(y, eye); */

  dest[8]  = z[0];
  dest[9]  = z[1];
  dest[10] = z[2];
  /* dest[11] = -vec3_dot(z, eye); */
}


