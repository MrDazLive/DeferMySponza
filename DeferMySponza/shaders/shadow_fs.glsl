#version 330

layout(location = 0)out float fragment_depth;

void main(void) {	
	fragment_depth = gl_FragCoord.z;
}