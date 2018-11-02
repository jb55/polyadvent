

//#include noise.glsl


// float clouds(vec3 position) {
//     float cloud_speed = 20000.0;
//     float cloud_pos = time * cloud_speed;
//     float cloud_trans = vec3(cloud_pos, cloud_pos, 0.0);

//     float clouds = snoise((position + cloud_trans)/800.0);
//     clouds *= max(0.0, dot(light_dir, vec3(0.0,0.0,1.0)));
//     clouds *= 0.35 * exp(clouds * -0.3);

//     return clouds;
// }

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}

vec3 standard_light(vec3 color, vec3 position, vec3 normal) {
	vec4 trans_normal = vec4(normal, 1);

    // vec3 light_dir = vec3()
    const float pi = 3.14159265;
    const float shiny = 12.0;
    const float exposure = 0.2;
    const float ambient_str = 0.2;
    float spec_str = 0.8 * light_intensity;

    // float light_intensity = light_intensity * 0.01;

    // too much ambient during daytime is making things look weird
    // ambient_str =- light_intensity * ambient_str;

    vec3 ray = camera_position - position;
    vec3 view_dir = normalize(ray);
    float distance = length(ray);
    float attenuation = 1.0 / distance;

    vec3 L = light_dir;
    vec3 N = normalize(trans_normal.xyz);
    float brightness = max(0.0, dot(L,N)) * light_intensity;


    // brightness += clouds(position);
    // brightness = clamp(brightness, 0.0, 1.0);

    vec3 diffuse = brightness * sun_color;
    vec3 ambient = ambient_str * sun_color;

    float spec;
    bool blinn = true;
    if (blinn) {
        const float energy_conservation = ( 8.0 + shiny ) / ( 8.0 * pi );
        vec3 halfway_dir = normalize(light_dir + view_dir);   // blinn-phong
        spec = energy_conservation * pow(max(dot(normal, halfway_dir), 0.0), shiny);
    }

    else {
        const float energy_conservation = ( 2.0 + shiny ) / ( 2.0 * pi );
        vec3 reflect_dir = reflect(-light_dir, normal); // phong
        spec = energy_conservation * pow(max(dot(view_dir, reflect_dir), 0.0), shiny);
    }
    // spec += pow(max(dot(view_dir, reflect_dir), 0.0), 16.0) * 0.5;

    vec3 specular = spec_str * spec * sun_color;
    vec3 final = (ambient + diffuse + specular) * color;

    // tone mapping
    // final = final / (vec3(1.0) - final * exposure);
    // final = final / (vec3(1.0) + color);

    return final;
}


