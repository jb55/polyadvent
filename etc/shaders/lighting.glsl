

#include noise.glsl



vec3 standard_light(vec3 color, vec3 position) {
	vec4 v4_normal = vec4(v_normal , 1);
	vec4 trans_normal = normal_matrix * v4_normal;

    vec3 L = light_dir * light_intensity;
    vec3 N = normalize(trans_normal.xyz);

    const float ambient_str = 0.1;
    const float spec_str = 0.5;

    float cloud_speed = 20000.0;
    float cloud_pos = time * cloud_speed;
    float cloud_trans = vec3(cloud_pos, cloud_pos, 0.0);

    float clouds = snoise((position + cloud_trans)/800.0);
    clouds *= max(0.0, dot(light_dir, vec3(0.0,0.0,1.0)));
    clouds *= 0.35 * exp(clouds * -0.3);

    float brightness = max(0.0, dot(L,N));

    // brightness += clouds;
    // brightness = clamp(brightness, 0.0, 1.0);

    vec3 diffuse = brightness * sun_color;
    vec3 ambient = ambient_str * sun_color;

    vec3 view_dir = normalize(camera_position - v_frag_pos);
    vec3 reflect_dir = normalize(light_dir + view_dir);   // blin-phong
    // vec3 reflect_dir = reflect(-light_dir, v_normal); // phong
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 16.0);

    vec3 specular = spec_str * spec * sun_color;

    return (ambient + diffuse + specular) * color;
}
