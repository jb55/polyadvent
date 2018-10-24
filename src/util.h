
#ifndef PA_UTIL_H
#define PA_UTIL_H

#include "gl.h"
#include "vec3.h"
#include "mat4.h"
#include <assert.h>


#define check_gl() {                                    \
        unsigned int e = glGetError();                  \
        if (e != GL_NO_ERROR) {                         \
            printf("Got error from OpenGL: %d\n", e);   \
            assert(0);                                  \
        }                                               \
    }

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

int clampi(int a, int mina, int maxa);
double clamp(double a, double mina, double maxa);
double max(double a, double b);
double min(double a, double b);

double rand_0to1();

#endif /* PA_UTIL_H */
