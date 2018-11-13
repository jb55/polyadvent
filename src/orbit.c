
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


static void orbit_node_update(struct node *node) {
    if (!node->parent)
        return;

    float *a = node->parent->mat;
    float *b = node->pos;
    float *dst = node->mat;

    float b30 = b[0], b31 = b[1], b32 = b[2], b33 = 1.0;

    float a00 = a[0],  a01 = a[1],  a02 = a[2],  a03 = a[3];
    float a10 = a[4],  a11 = a[5],  a12 = a[6],  a13 = a[7];
    float a20 = a[8],  a21 = a[9],  a22 = a[10], a23 = a[11];
    float a30 = a[12], a31 = a[13], a32 = a[14], a33 = a[15];


    dst[12] = b30*a00 + b31*a10 + b32*a20 + b33*a30;
    dst[13] = b30*a01 + b31*a11 + b32*a21 + b33*a31;
    dst[14] = b30*a02 + b31*a12 + b32*a22 + b33*a32;
    dst[15] = b30*a03 + b31*a13 + b32*a23 + b33*a33;
}


void init_orbit(struct orbit *orbit) {
    node_init(&orbit->node);
    orbit->node.label = "orbit_camera";
    /* orbit->node.custom_update = orbit_node_update; */
    /* orbit->node.custom_update_data = orbit; */
}
