#version 330

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texture_coordinate;

flat in int varying_material;

out vec4 fragment_colour;

void main(void) {
	if(varying_material == 200)
		fragment_colour.rgb = vec3(1, 1, 1);
	else if(varying_material == 201)
		fragment_colour.rgb = vec3(1, 1, 0);
	else if(varying_material == 202)
		fragment_colour.rgb = vec3(1, 0, 1);
	else if(varying_material == 203)
		fragment_colour.rgb = vec3(1, 0, 0);
	else if(varying_material == 204)
		fragment_colour.rgb = vec3(0, 1, 1);
	else if(varying_material == 205)
		fragment_colour.rgb = vec3(0, 1, 0);
	else if(varying_material == 206)
		fragment_colour.rgb = vec3(0, 0, 1);
	else
		fragment_colour.rgb = vec3(0, 0, 0);

	//float val = (varying_material - 200) / 6;
	//fragment_colour.rgb = vec3(val) + vec3(1);
	//fragment_colour.rgb = varying_normal + vec3(1);
	//fragment_colour.rgb /= 2;
	fragment_colour *= 0.7f;
}