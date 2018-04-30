
#ifndef PA_UTIL_H
#define PA_UTIL_H

#include "gl.h"
#include "vec3.h"
#include "mat4.h"
#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))

void check_gl(void);
int clampi(int a, int mina, int maxa);
double clamp(double a, double mina, double maxa);
double max(double a, double b);
double min(double a, double b);

void look_at(vec3 *eye, vec3 *target, vec3 *up, mat4 *dest);

double rand_0to1();

#endif /* PA_UTIL_H */
