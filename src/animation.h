
#ifndef POLYADVENT_ANIMATION_H
#define POLYADVENT_ANIMATION_H



#define MAX_JOINT_CHILDREN 3

struct joint
{
    int children[MAX_JOINT_CHILDREN];
    int id;
    float *mat;
};



struct animated_model
{
    int root_joint;
};


void collada_test();


#endif /* POLYADVENT_ANIMATION_H */
