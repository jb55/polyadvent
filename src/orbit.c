
#include "orbit.h"
#include "node.h"
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

