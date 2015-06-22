
typedef float mat4;

mat4 *mat4_transpose(mat4 *src, mat4 *dest);
mat4 *mat4_inverse(mat4 *src, mat4 *dest);
mat4 *mat4_copy(const mat4 *src, mat4 *dst);
mat4 *mat4_id(mat4 *dest);
mat4 *mat4_multiply(const mat4 *a, const mat4 *b, mat4 *dest);

mat4 *mat4_rotate(const mat4 *mat, const float angle,
                  const float *v3_axis, mat4 *dest);
