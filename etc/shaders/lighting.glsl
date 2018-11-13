

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
const float PI = 3.14159265;

vec3 gamma_correct(vec3 color) {
    return pow(color, vec3(1.0/2.2));
}

vec3 uncharted_tonemap(const vec3 x) {
	const float A = 0.15;
	const float B = 0.50;
	const float C = 0.10;
	const float D = 0.20;
	const float E = 0.02;
	const float F = 0.30;
	return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

vec3 standard_light(vec3 color, vec4 position, vec4 normal) {
    // vec3 light_dir = vec3()
    const float shiny = 14.0;
    const float exposure = 0.3;
    float ambient_str = 0.2;
    float spec_str = 0.8 * light_intensity;

    vec4 trans_normal = normal_matrix * normal;
    vec3 L = light_dir;
    vec3 N = normalize(trans_normal.xyz);
    float cos_theta = max(0.0, dot(L,N));

    // float light_intensity = light_intensity * 0.01;

    // too much ambient during daytime is making things look weird
    // ambient_str =- light_intensity * ambient_str * 0.05;

    vec3 ray = camera_position - position.xyz;
    vec3 view_dir = normalize(ray);

    float brightness = cos_theta * light_intensity;


    // brightness += clouds(position);
    // brightness = clamp(brightness, 0.0, 1.0);

    vec3 diffuse = brightness * sun_color;
    vec3 ambient = ambient_str * sun_color;

    float spec;
    bool blinn = true;
    if (blinn) {
        const float energy_conservation = ( 8.0 + shiny ) / ( 8.0 * PI );
        vec3 halfway_dir = normalize(light_dir + view_dir);   // blinn-phong
        spec = energy_conservation * pow(max(dot(normal.xyz, halfway_dir), 0.0), shiny);
    }

    else {
        const float energy_conservation = ( 2.0 + shiny ) / ( 2.0 * PI );
        vec3 reflect_dir = reflect(-light_dir, normal.xyz); // phong
        spec = energy_conservation * pow(max(dot(view_dir, reflect_dir), 0.0), shiny);
    }
    // spec += pow(max(dot(view_dir, reflect_dir), 0.0), 16.0) * 0.5;

    vec3 specular = spec_str * spec * sun_color;
    vec3 final = (ambient + diffuse + specular) * color;

    // tone mapping
    // final = final / (vec3(1.0) - final * exposure);
    // final = final / (vec3(1.0) + color);
    // final = uncharted_tonemap(final);

    return final;
}


#include pbr.glsl
