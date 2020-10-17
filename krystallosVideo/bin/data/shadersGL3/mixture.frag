#version 150

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform float mixture;

in vec2 texCoordVarying;

out vec4 outputColor;

void main() {
	vec4 texel0 = texture(tex0, texCoordVarying);
	vec4 texel1 = texture(tex1, texCoordVarying);
	outputColor = texel0 * mixture + texel1 * (1 - mixture);
}
