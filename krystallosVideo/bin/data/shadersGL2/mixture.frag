#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect tex1;
uniform float mixture;

void main() {

	vec2 texCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
	vec4 texel0 = texture2DRect(tex0, texCoord);
	vec4 texel1 = texture2DRect(tex1, texCoord);
	
	gl_FragColor = texel0 * mixture + texel1 * (1 - mixture);
	
}
