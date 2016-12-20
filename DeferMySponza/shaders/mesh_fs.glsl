#version 330

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texture_coordinate;

out vec4 fragment_colour;


void main(void) {
	fragment_colour.rgb = varying_normal + vec3(1);
	fragment_colour.rgb /= 2;
	fragment_colour *= 0.7f;
}