#version 330

struct Material {
	vec3 diffuse;
	int id;
	vec3 specular;
	float shine;
};

layout (std140) uniform block_material {
	Material material[7];
};

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texture_coordinate;

flat in int varying_material;

out vec4 fragment_colour;

void main(void) {
	if(varying_material == 200)
		fragment_colour.rgb = vec3(1, 1, 1);	///WALLS
	else if(varying_material == 201)
		fragment_colour.rgb = vec3(1, 1, 0);	///ROOF
	else if(varying_material == 202)
		fragment_colour.rgb = vec3(1, 0, 1);	///DRAPES
	else if(varying_material == 203)
		fragment_colour.rgb = vec3(1, 0, 0);	///POLES
	else if(varying_material == 204)
		fragment_colour.rgb = vec3(0, 1, 1);	///BUDAH
	else if(varying_material == 205)
		fragment_colour.rgb = vec3(0, 1, 0);	///RABIT
	else if(varying_material == 206)
		fragment_colour.rgb = vec3(0, 0, 1);	///DRAGON
	else
		fragment_colour.rgb = vec3(0, 0, 0);


	//fragment_colour.rgb = material[0].diffuse;

	//float val = (varying_material - 200) / 6;
	//fragment_colour.rgb = vec3(val) + vec3(1);
	//fragment_colour.rgb = varying_normal + vec3(1);
	//fragment_colour.rgb /= 2;
	fragment_colour *= 0.7f;
}