#version 150

uniform sampler2DRect tex0;
uniform float r1;
uniform float g1;
uniform float b1;
uniform float r2;
uniform float g2;
uniform float b2;
uniform float width;
uniform float mixture;

in vec2 texCoordVarying;

out vec4 outputColor;

void main() {

    // gl_FragCoord contains the window relative coordinate for the fragment.
    // we use gl_FragCoord.x position to control the red color value.
    // we use gl_FragCoord.y position to control the green color value.
    // please note that all r, g, b, a values are between 0 and 1.
    
    vec4 texel0 = texture(tex0, texCoordVarying);
    vec4 color;
    
    // color 1
    if(mod(gl_FragCoord.x, width*2) > width) {
    	color = vec4(r1, g1, b1, 1);
    	
    // color 2
    } else {
    	color = vec4(r2, g2, b2, 1);
    	
    }

	outputColor = texel0 * mixture + color * (1 - mixture);
    
}