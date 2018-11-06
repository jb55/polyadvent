
#ifndef ORBIT_H
#define ORBIT_H

struct node;

struct orbit {
    float dist;
    union {
        struct {
            float yaw;
            float pitch;
            float roll;
        };

        float ypr[3];
    } angles;
};


void orbit_to_node(struct orbit *orbit, struct node *node);
void orbit_to_quat(struct orbit *, float *a);


#endif /* ORBIT_H */
