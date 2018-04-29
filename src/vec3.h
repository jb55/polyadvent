typedef float vec3;

#define V3(x,y,z) ((vec3[3]){x,y,z})
#define V_X 0
#define V_Y 1
#define V_Z 2

vec3 *vec3_scale(vec3 *vec, float val, vec3 *dest);
vec3 *vec3_subtract(vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_cross (vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_multiply(vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_scale(vec3 *vec, float val, vec3 *dest);
vec3 *vec3_normalize(vec3 *vec, vec3 *dest);
vec3 *vec3_length(vec3 *vec, vec3 *dest);
vec3 *vec3_lengthsq(vec3 *vec, vec3 *dest);

