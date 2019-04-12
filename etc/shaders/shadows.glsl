
vec3 shadow_strength(vec4 position, vec4 normal, vec4 v_shadow_coord) {
    vec3 visibility = vec3(1.0);
    // vec4 world_space_normal = normalize(normal * normal);
    // float cos_theta = max(0.0, dot(light_dir, world_space_normal.xyz));
    // float shadow_normal_offset = 1.0;
    // float normal_offset_scale = clamp(1.0 - cos_theta, 0.0, 1.0);
    // normal_offset_scale *= shadow_normal_offset * 0.01;
    // vec4 shadow_offset = world * (world_space_normal * normal_offset_scale);
    // vec4 shadow_coord = depth_vp * ((world * position) + shadow_offset);
    vec4 shadow_sample = texture(shadow_map, v_shadow_coord.xy);

    float light_angle = dot(light_dir, normal.xyz);
    float bias = 0.002 * 1.0/light_angle;

    bool in_shadow =
        shadow_sample.z < v_shadow_coord.z - bias
        && shadow_sample.y < 1.0;

    if (light_dir.z > 0.0 && in_shadow) {
        float factor = 1.0-abs(light_angle);
        // float factor = 1.0;
        visibility = vec3(1.0)*factor;
        // visibility = shadow_sample;
    }

    // visibility += normal_offset_scale;


    return visibility;
}
