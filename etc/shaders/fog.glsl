
vec3 apply_fog(in vec3 rgb, in float distance, in vec3 ray_orig, in vec3 ray_dir) {
    const float b = 0.00035;

    float draw_dist = 1.4;
    float fog_amount = 1.0-exp(-pow(distance * (1.0/draw_dist) * b, 6.0));
    // float sun_amount = max( dot( ray_dir, -light_dir ), 0.0 ) * b;
    // sun_amount = pow(sun_amount, 0.0);

    vec3 fog_color = sun_color * sky_intensity;
    // vec3 fog_color = sun_color;

    // vec3  fog_color  = mix(vec3(0.5,0.6,0.7),
    //                        vec3(1.0,0.9,0.8),
    //                 smoothstep(0.1, 1.0, sun_amount)
    //                        ) * light_intensity;

    // vec3 final_color = rgb*(1.0-exp(-distance*b)) + fog_color*exp(-distance*b);
    vec3 final_color = mix( rgb, fog_color, fog_amount);

    return final_color;
}


vec3 depth_fog(vec3 rgb, vec4 depth) {
    return depth.xyz;
}
