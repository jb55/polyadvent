
#include "orbit.h"
#include "node.h"
#include "vec3.h"
#include <math.h>

void orbit_to_mat4(struct orbit *orbit, float *m)
{
}


static void orbit_to_vec3(struct orbit *orbit, float *v3)
{
    float x = r * sin(theta) * cos(phi);
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

