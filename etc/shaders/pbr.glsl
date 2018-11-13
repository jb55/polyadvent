
float dist_ggx(vec3 N, vec3 H, float roughness)
{
    float a      = roughness * roughness;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float num    = a;
    float denom  = (NdotH2 * (a - 1.0) + 1.0);
    denom        = PI * denom * denom;

    return num / denom;
}


float geom_schlick_ggx(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float num   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return num / denom;
}

float geom_smith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx1 = geom_schlick_ggx(NdotV, roughness);
    float ggx2 = geom_schlick_ggx(NdotL, roughness);

    return ggx1 * ggx2;
}

vec3 fresnel_schlick(float cos_theta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cos_theta, 5.0);
}

vec3 pbr(vec3 albedo, vec3 V, vec3 normal) {
    const float ao = 12.0;
    const float metallic = 0.6;
    const float roughness = 0.6;
    vec3 radiance = sun_color * light_intensity * 2.0;
    // radiance += texture(skybox, vec3(0.0, 0.0, 1.0)).rgb;
    vec3 N = normalize(normal);
    vec3 L = normalize(light_dir);
    vec3 H = normalize(V + L);
    // non-metallic is always 0.04
    vec3 F0 = vec3(0.04);
    // while we do vary F0 based on the metalness of a surface by linearly
    // interpolating between the original F0 and the albedo value given the
    // metallic property
    F0 = mix(F0, albedo, metallic);
    float HdotV     = max(0.0, dot(H, V));
    vec3  F         = fresnel_schlick(HdotV, F0);
    float NDF       = dist_ggx(N, H, roughness);
    float G         = geom_smith(N, V, L, roughness);

    float NdotL = max(0.0, dot(N, L));

    // Cook-Torrance BRDF
    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(0.0, dot(N, V)) * NdotL;
    vec3 specular     = numerator / max(denominator, 0.001);

    // kS = energy of light that gets reflected
    // kD = remaining energy that gets refracted
    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - metallic;

    // because metallic surfaces don't refract light and thus have no diffuse
    // reflections we enforce this property by nullifying kD if the surface is
    // metallic

    // Lo = outgoing radiance
    // the result of the reflectance equation's integral ∫ over Ω

    vec3 Lo = (kD * albedo / PI + specular) * radiance * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color   = ambient + Lo;
    // color = uncharted_tonemap(color);
    // color = color / (vec3(1.0) + color);

    return color;
}
