vec4 v4_pos = vec4(position, 1.0);
gl_Position = mvp * v4_pos;
v_normal = normal;
v_color_smooth = v_color = color;
shadow_coord = depth_mvp * v4_pos;
v_position = position;
v_frag_pos = (world * v4_pos).xyz;
