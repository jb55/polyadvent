
#include "orbit.h"
#include "node.h"
#include "vec3.h"
#include <math.h>

void orbit_to_quat(struct orbit *orbit, float *quat)
{
    float phi = orbit->azimuth;
    float theta = orbit->inclination;

    // vector
    quat[0] =  cos(phi/2.0)*cos(theta/2.0);  // scalar quaternion components
    quat[1] = -sin(phi/2.0)*sin(theta/2.0);  // x quaternion components
    quat[2] =  cos(phi/2.0)*sin(theta/2.0);  // y quaternion components
    quat[3] =  sin(phi/2.0)*cos(theta/2.0);  // z quaternion components
}

/* static void orbit_update_node(struct node *node) { */
/* } */

void orbit_to_node(struct orbit *orbit, struct node *node) {
    orbit_to_quat(orbit, node->orientation);
    node_mark_for_recalc(node);
}


static void arcball_vector(int screen_width, int screen_height, int x, int y, float *p)
{
    p[0] = x/screen_width*2.0 - 1.0;
    p[1] = -y/screen_height*2.0 - 1.0;
    p[2] = 0.0;

    float xy_sq = p[0]*p[0] + p[1]*p[1];
    if (xy_sq < 1.0)
        p[2] = sqrt(-xy_sq);
    else
        vec3_normalize(p, p);
}

