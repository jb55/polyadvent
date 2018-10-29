
vec3 standard_light(vec3 color) {
	vec4 v4_normal = vec4(normal , 1);
	vec4 trans_normal = normal_matrix * v4_normal;

    vec3 L = light_dir;
    vec3 N = normalize(trans_normal.xyz);

    float costheta = clamp(dot(L,N), 0.3, 1.0) * light_intensity;

    return color * costheta;
}

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}
