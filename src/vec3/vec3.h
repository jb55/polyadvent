typedef float vec3;

#define V3(x,y,z) ((float[3]){x,y,z})

vec3 *vec3_scale(vec3 *vec, double val, vec3 *dest);
vec3 *vec3_subtract(vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_cross (vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_multiply(vec3 *vec, vec3 *vec2, vec3 *dest);
vec3 *vec3_scale(vec3 *vec, double val, vec3 *dest);
vec3 *vec3_normalize(vec3 *vec, vec3 *dest);

