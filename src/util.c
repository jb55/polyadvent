
#include "util.h"
#include "vec3.h"
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

void glhLookAtf2( float *matrix, float *eyePosition3D,
                  float *center3D, float *upVector3D )
{
   float forward[3], side[3], up[3];
   float matrix2[16], resultMatrix[16];
   // --------------------
   forward[0] = center3D[0] - eyePosition3D[0];
   forward[1] = center3D[1] - eyePosition3D[1];
   forward[2] = center3D[2] - eyePosition3D[2];
   vec3_normalize(forward, forward);
   // --------------------
   // Side = forward x up
   ComputeNormalOfPlane(side, forward, upVector3D);
   NormalizeVector(side);
   --------------------
   // Recompute up as: up = side x forward
   ComputeNormalOfPlane(up, side, forward);
   // --------------------
   matrix2[0] = side[0];
   matrix2[4] = side[1];
   matrix2[8] = side[2];
   matrix2[12] = 0.0;
   // --------------------
   matrix2[1] = up[0];
   matrix2[5] = up[1];
   matrix2[9] = up[2];
   matrix2[13] = 0.0;
   // --------------------
   matrix2[2] = -forward[0];
   matrix2[6] = -forward[1];
   matrix2[10] = -forward[2];
   matrix2[14] = 0.0;
   // --------------------
   matrix2[3] = matrix2[7] = matrix2[11] = 0.0;
   matrix2[15] = 1.0;
   // --------------------
   MultiplyMatrices4by4OpenGL_FLOAT(resultMatrix, matrix, matrix2);
   glhTranslatef2(resultMatrix,
                  -eyePosition3D[0], -eyePosition3D[1], -eyePosition3D[2]);
   // --------------------
   memcpy(matrix, resultMatrix, 16*sizeof(float));
}
