shader_type spatial;

render_mode unshaded;

uniform vec3 light_direction = vec3(0, 0, 1);

uniform vec4 color_lit : source_color = vec4(1.0);

uniform vec4 color_shadow : source_color = vec4(0.1, 0.1, 0.1, 1.0);

uniform vec4 color_ceil : source_color = vec4(0.0, 1.0, 0.2, 1.0);

uniform vec4 color_bottom : source_color = vec4(0.1, 0.1, 0.1, 1.0);

uniform float y_ceil = 0.5;

uniform float y_min = -1;

void vertex() {
	vec3 light_dir = normalize(light_direction);
	float light = dot(-light_dir, normalize(NORMAL.xyz));
	// map light value [-1,1] to [0,1]
	light = (light + 1.0) * 0.5;
	COLOR = mix(color_lit, color_shadow, light);

	// fade to color_bottom
	float fade = smoothstep(y_min, y_ceil, VERTEX.y);
	float above_ceil = smoothstep(y_ceil - 0.1, y_ceil + 0.1, VERTEX.y);
	fade = mix(fade * 0.7, 1.0, above_ceil);
	COLOR = mix(color_bottom, COLOR, fade);

	//// draw a line at the ceil
	//float ceilish = max(0.1 - abs(VERTEX.y - y_ceil), 0);
	//ceilish = smoothstep(0, 0.0001, ceilish);
	//COLOR = mix(COLOR, COLOR * 0.5 + color_ceil, ceilish * 0.75);
}

vec3 matrixMult(mat4 matrix, vec3 vector) {
  return (matrix * vec4(vector, 1.0)).xyz;
}

void fragment() {
	vec3 vert_model = VERTEX;
	vert_model = matrixMult(INV_VIEW_MATRIX, vert_model);
	vert_model = matrixMult(inverse(MODEL_MATRIX), vert_model);

	ALBEDO = COLOR.xyz;

	// draw a line at the ceil
	float ceilish = max(0.05 - abs(vert_model.y - y_ceil), 0);
	ceilish = smoothstep(0, 0.01, ceilish);
	ALBEDO = mix(ALBEDO, ALBEDO * 0.5 + color_ceil.rgb, ceilish * 0.75);
}
