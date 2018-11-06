
#include "orbit.h"
#include "node.h"
#include "vec3.h"
#include "quat.h"
#include <math.h>

void orbit_to_mat4(struct orbit *orbit, float *m)
{
    quat_axis_angle(orbit->angles.ypr)
}

/* static void orbit_update_node(struct node *node) { */
/* } */

void orbit_to_node(struct orbit *orbit, struct node *node) {
    /* orbit_to_quat(orbit, node->orientation); */
    node_mark_for_recalc(node);
}


