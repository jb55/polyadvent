

#ifndef POLYADVENT_QUAT4_
#define POLYADVENT_QUAT4_

typedef float quat;

void quat_id(quat *dest);
void quat_multiply(quat *a, quat *b, quat *dest);
void quat_axis_angle(float *axis, float angle, quat *dest);

#endif /* POLYADVENT_QUAT4_ */
