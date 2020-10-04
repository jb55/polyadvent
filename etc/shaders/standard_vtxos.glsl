vec4 v4_pos = vec4(position, 1.0);
// data_out.normal = normal;
// data_out.position = position;

data_out.normal = mat3(transpose(inverse(model))) * normal;
data_out.position = vec3(model * v4_pos);
data_out.color_smooth = data_out.color = color;
data_out.shadow_coord = depth_mvp * v4_pos;
// TODO: move shadow coord calc from frag to here
