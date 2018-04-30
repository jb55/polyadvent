
typedef float mat4;

#define M_X 12
#define M_Y 13
#define M_Z 14


/* mat4 *mat4_create_transform(mat4 *a, float *pos, float *scale, float *rot); */
mat4 *mat4_scale(mat4 *a, float v[3], mat4 *out);
mat4 *mat4_frustum (float left, float right, float bottom,
                    float top, float near, float far, mat4 *dest);

mat4 *mat4_perspective(float fov, float aspect, float near,
                       float far, mat4 *dest);

mat4 *mat4_translate (mat4 *mat, float *vec3, mat4 *dest);
mat4 *mat4_transpose(mat4 *src, mat4 *dest);
mat4 *mat4_inverse(mat4 *src, mat4 *dest);
mat4 *mat4_copy(const mat4 *src, mat4 *dst);
mat4 *mat4_id(mat4 *dest);
mat4 *mat4_multiply(const mat4 *a, const mat4 *b, mat4 *dest);
void mat4_print(const mat4 *a);

mat4 *mat4_rotate(const mat4 *mat, const float angle,
                  const float *v3_axis, mat4 *dest);
