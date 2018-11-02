vec4 v4_pos = vec4(position, 1.0);
vertex.normal = normal;
vertex.color_smooth = vertex.color = color;
vertex.shadow_coord = depth_mvp * v4_pos;
vertex.position = v4_pos.xyz;
vertex.frag_pos = (world * v4_pos).xyz;
