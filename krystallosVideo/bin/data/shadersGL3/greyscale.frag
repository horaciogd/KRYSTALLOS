#version 150

uniform sampler2DRect tex0;
uniform float threshold;

in vec2 texCoordVarying;

out vec4 outputColor;

void main() {
	   
	vec4 texel0 = texture(tex0, texCoordVarying);
	float grey = 0.2126 * texel0.r + 0.7152 * texel0.g + 0.0722 * texel0.b;
	
	if (grey<threshold) {
		outputColor = vec4(grey, grey, grey, texel0.a);
	} else {
		outputColor = vec4(texel0.r, texel0.g, texel0.b, texel0.a);
	}
	
}
