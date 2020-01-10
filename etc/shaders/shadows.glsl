
float texture2DCompare(sampler2D depths, vec2 uv, float compare){
  float depth = texture2D(depths, uv).r;
  return step(compare, depth);
}

float texture2DShadowLerp(sampler2D depths, vec2 size, vec2 uv, float compare){
  vec2 texelSize = vec2(1.0)/size;
  vec2 f = fract(uv*size+0.5);
  vec2 centroidUV = floor(uv*size+0.5)/size;

  float lb = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 0.0), compare);
  float lt = texture2DCompare(depths, centroidUV+texelSize*vec2(0.0, 1.0), compare);
  float rb = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 0.0), compare);
  float rt = texture2DCompare(depths, centroidUV+texelSize*vec2(1.0, 1.0), compare);
  float a = mix(lb, lt, f.y);
  float b = mix(rb, rt, f.y);
  float c = mix(a, b, f.x);
  return c;
}

float PCF(sampler2D depths, vec2 size, vec2 uv, float compare){
  float result = 0.0;
  for(int x=-1; x<=1; x++){
    for(int y=-1; y<=1; y++){
      vec2 off = vec2(x,y)/size;
      result += texture2DShadowLerp(depths, size, uv+off, compare);
    }
  }
  return result/9.0;
}

vec3 shadow_strength(vec4 position, vec4 normal, vec4 v_shadow_coord) {
    vec3 visibility = vec3(1.0);
    vec4 shadow_sample = texture(shadow_map, v_shadow_coord.xy);

    float light_angle = dot(light_dir, normal.xyz);
    float bias = 0.002;

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
