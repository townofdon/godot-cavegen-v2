shader_type spatial;
render_mode unshaded;

uniform sampler2D TEXTURE;

void fragment() {
	ALBEDO = texture(TEXTURE, UV).rgb;
}
