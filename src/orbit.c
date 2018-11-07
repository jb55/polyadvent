
#include "orbit.h"
#include "node.h"
#include "vec3.h"
#include "mat4.h"
#include "quat.h"
#include "util.h"
#include <math.h>

float *spherical_to_cartesian(struct spherical *s, float *v3)
{
    float theta = s->inclination;
    float phi   = s->azimuth;

    float sin_theta = sin(theta);
    float cos_phi   = cos(phi);
    float cos_theta = cos(theta);

    // to cartesian
    v3[0] = s->radius * sin_theta * cos_phi;
    v3[1] = s->radius * sin_theta * sin(phi);
    v3[2] = s->radius * cos_theta;

    return v3;
}

vec3 *spherical_dir(struct spherical s, vec3 *dir) {
    s.radius = 1.0;
    spherical_to_cartesian(&s, dir);
    return dir;
}

// from: in
// to:   out
vec3 *spherical_pos(struct spherical *s, vec3 *from, vec3 *to) {
    float spherical_offset[3];
    spherical_to_cartesian(s, spherical_offset);
    vec3_add(from, spherical_offset, to);
    return to;
}

vec3 *spherical_look_at(struct spherical *s, vec3 *target, mat4 *mat) {
    float eye[3];

    spherical_pos(s, target, eye);
    look_at(eye, target, V3(0.0, 0.0, 1.0), mat);
    return mat;
}



