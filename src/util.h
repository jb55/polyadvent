
#ifndef PA_UTIL_H
#define PA_UTIL_H

#include "gl.h"
#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void check_gl(void);
int clampi(int a, int mina, int maxa);
double clamp(double a, double mina, double maxa);

double rand_0to1();

#endif /* PA_UTIL_H */
