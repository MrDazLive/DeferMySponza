#version 330

uniform	sampler2DRect frame;

out vec3 fragment_colour;

void main(void) {
	fragment_colour = texture(frame, gl_FragCoord.xy).rgb * vec3(1, 0.3f, 0.3f);
}