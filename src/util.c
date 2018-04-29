
#include "util.h"
#include "vec3/vec3.h"
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

mat4 *look_at(vec3 *eye, vec3 *target, vec3 *up) {
  float[3] z;
  vec3_subtract(eye, target, z);
  if (vec3_lengthsq(z) == 0)
    z[V_Z] = 0;
}

