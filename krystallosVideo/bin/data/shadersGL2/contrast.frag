#version 120

uniform sampler2DRect tex0;
uniform sampler2DRect texEr;
uniform sampler2DRect texEg;
uniform sampler2DRect texEb;
uniform float fr;
uniform float fg;
uniform float fb;

uniform float mixture;

void main() {

	// gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    
	vec2 texCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec4 texel0 = texture2DRect(tex0, texCoord);
    
	vec2 posR = vec2(texel0.r*255, fr);
	vec2 posG = vec2(texel0.g*255, fg);
	vec2 posB = vec2(texel0.b*255, fb);
	
	vec4 texelR = texture2DRect(texEr, posR);
	vec4 texelG = texture2DRect(texEg, posG);
	vec4 texelB = texture2DRect(texEb, posB);
    
    vec4 contrast = vec4(texelR.r, texelG.g, texelB.b, texel0.a);
	
    gl_FragColor = contrast * mixture + texel0 * (1 - mixture);
	
}
