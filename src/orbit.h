
#ifndef ORBIT_H
#define ORBIT_H

struct node;

struct orbit {
    float radius;
    float inclination;
    float azimuth;
};


void orbit_to_node(struct orbit *orbit, struct node *node);
void orbit_to_quat(struct orbit *, float *a);


#endif /* ORBIT_H */
