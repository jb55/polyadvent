vec4 v4_pos = vec4(position, 1.0);
data_out.normal = normal;
data_out.position = position;
data_out.color_smooth = data_out.color = color;
data_out.shadow_coord = depth_mvp * v4_pos;
data_out.frag_pos = (world * v4_pos).xyz;
// TODO: move shadow coord calc from frag to here
