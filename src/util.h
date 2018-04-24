
#ifndef PA_UTIL_H
#define PA_UTIL_H

#include "gl.h"
#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

typedef struct vec3 {
  union {
    float x,y,z;
    float xyz[3];
  };
} vec3;

void check_gl(void);

double rand_0to1();

#endif /* PA_UTIL_H */
