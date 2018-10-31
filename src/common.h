
#ifndef POLYADVENT_COMMON_H
#define POLYADVENT_COMMON_H

#include <stdbool.h>

#define MAT3_ELEMS 9
#define MAT4_ELEMS 16

#define SLAB(f) "etc/slab/" f
#define RESOURCE(f) "etc/shaders/" f

typedef unsigned char u8;
typedef signed char s8;

typedef unsigned short u16;
typedef signed short s16;

typedef unsigned int u32;
typedef signed int s32;

typedef unsigned long long u64;
typedef signed long long s64;

struct point {
  double x, y;
};


#endif /* POLYADVENT_COMMON_H */
