typedef float vec3;

#define V3(x,y,z) ((vec3[3]){x,y,z})
#define V_X 0
#define V_Y 1
#define V_Z 2

vec3 *vec3_direction (vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_scale(vec3 *vec, float val, vec3 *dest);
vec3 *vec3_subtract(vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_cross (vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_multiply(vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_scale(vec3 *vec, float val, vec3 *dest);
vec3 *vec3_normalize(vec3 *vec, vec3 *dest);
float vec3_dist(vec3 *vec, vec3 *vec2);
float vec3_distsq(vec3 *vec, vec3 *vec2);
float *vec3_set(vec3 *vec, vec3 *dest);
float vec3_length(vec3 *vec);
float vec3_lengthsq(vec3 *vec);
float vec3_dot(vec3 *vec, vec3 *vec2);
int vec3_isall(vec3 *vec, float n);
vec3 *vec3_all(vec3 *vec, float n);
vec3 *vec3_add(vec3 *vec, vec3 *vec2, vec3 *dest);
int vec3_eq(vec3 *a, vec3 *b, float precision);
void vec3_copy(vec3 *a, vec3 *dest);
