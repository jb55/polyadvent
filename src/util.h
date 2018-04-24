
#ifndef PA_UTIL_H
#define PA_UTIL_H

#include "gl.h"
#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void check_gl(void);

float rand_0to1();

#endif /* PA_UTIL_H */
