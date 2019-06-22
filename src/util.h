
#ifndef PA_UTIL_H
#define PA_UTIL_H

#include "gl.h"
#include "vec3.h"
#include "mat4.h"
#include <assert.h>
#include <string.h>

#define check_gl() {                                    \
        unsigned int e = glGetError();                  \
        if (e != GL_NO_ERROR) {                         \
            printf("Got error from OpenGL: %d\n", e);   \
            assert(0);                                  \
        }                                               \
    }

#define UP_VEC V3(0.0, 0.0, 1.0)
#define PI 3.14159265
#define TAU 6.2831853

#define RAD(x) ((x)*TAU/360.0)

#define EPSILON 0.0001
#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define min(a,b) (a < b ? a : b)
#define max(a,b) (a > b ? a : b)
#define streq(a,b) (strcmp(a,b) == 0)
#define memeq(a,b,n1,n2) (memcmp(a,b,min(n1, n2)) == 0)
#define approxeq(a, b) (fabs(a-b) < EPSILON)

void look_at(vec3 *eye, vec3 *target, vec3 *up, mat4 *dest);
int clampi(int a, int mina, int maxa);
double clamp(double a, double mina, double maxa);

double rand_0to1();

#endif /* PA_UTIL_H */
