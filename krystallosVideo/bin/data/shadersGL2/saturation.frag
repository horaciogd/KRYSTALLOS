#version 120

uniform sampler2DRect tex0;
uniform float threshold;

void main() {
	   
	vec2 texCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
	vec4 texel0 = texture2DRect(tex0, texCoord);
	float grey = 0.2126 * texel0.r + 0.7152 * texel0.g + 0.0722 * texel0.b;
	
	if (grey<threshold) {
		gl_FragColor = vec4(0, 0, 0, texel0.a);
	} else {
		gl_FragColor = vec4(texel0.r, texel0.g, texel0.b, texel0.a);
	}
	
}
