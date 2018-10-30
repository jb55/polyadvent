
vec3 standard_light(vec3 color) {
	vec4 v4_normal = vec4(normal , 1);
	vec4 trans_normal = normal_matrix * v4_normal;

    vec3 L = light_dir;
    vec3 N = normalize(trans_normal.xyz);

    float costheta = clamp(dot(L,N), 0.3, 1.0) * light_intensity;

    return color * costheta;
}


const vec3 spherical_harmonics[4] = vec3[](
  vec3( 0.754554516862612,  0.748542953903366,  0.790921515418539 ),
  vec3( -0.083856548007422,  0.092533500963210,  0.322764661032516 ),
  vec3( 0.308152705331738,  0.366796330467391,  0.466698181299906 ),
  vec3( -0.188884931542396, -0.277402551592231, -0.377844212327557 )
);

vec3 irradiance_spherical_harmonics(const vec3 n) {
    return max(
               spherical_harmonics[0]
               + spherical_harmonics[1] * n.y
               + spherical_harmonics[2] * n.z
               + spherical_harmonics[3] * n.x
               , 0.0);
}

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}
