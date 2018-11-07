
#ifndef ORBIT_H
#define ORBIT_H

#include "vec3.h"
#include "node.h"
#include "mat4.h"

struct spherical {
    float radius;
    float inclination;
    float azimuth;
};

struct orbit {
    struct spherical coords;
    struct node node;
};

float *spherical_pos(struct spherical *s, vec3 *from, vec3 *to);
float *spherical_look_at(struct spherical *s, vec3 *target, mat4 *mat);
float *spherical_to_cartesian(struct spherical *s, float *v3);

#endif /* ORBIT_H */
