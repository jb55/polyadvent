

#ifndef POLYADVENT_QUAT4_
#define POLYADVENT_QUAT4_

typedef float quat;
#define QUAT(x,y,z,w) ((quat[4]){x,y,z,w})

void quat_id(quat *dest);
void quat_multiply(quat *a, quat *b, quat *dest);
void quat_axis_angle(float *axis, float angle, quat *dest);
void quat_from_axes(float yaw, float pitch, float roll);
void quat_to_mat3(quat *quat, float *dest);
void quat_multiply_vec3(quat *quat, float *vec, float *dest);
quat *quat_inverse(quat *q, quat *dest);

#endif /* POLYADVENT_QUAT4_ */
