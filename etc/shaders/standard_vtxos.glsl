vec4 v4_pos = vec4(position, 1.0);
gl_Position = mvp * v4_pos;
vertex.normal = normal;
vertex.color = vertex.color_smooth = color;
vertex.shadow_coord = depth_mvp * v4_pos;
vertex.position = position;
vertex.frag_pos = (world * v4_pos).xyz;
