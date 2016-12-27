#version 330

struct Material {
	vec3 diffuse;
	float shine;

	vec3 specular;
	int mainTexture;

	vec2 excess;
	int normalTexture;
	int excess_;
};

layout (std140) uniform block_material {
	Material material[7];
};

uniform	sampler2D mainTexture[7];
uniform	sampler2D normalTexture[7];

in vec3 varying_position;
in vec3 varying_normal;
in vec2 varying_texture_coordinate;

flat in int fixed_material;

out vec4 fragment_colour;

void main(void) {
	/*if(fixed_material == 200)
		fragment_colour.rgb = vec3(1, 1, 1);	///WALLS
	else if(fixed_material == 201)
		fragment_colour.rgb = vec3(1, 1, 0);	///ROOF
	else if(fixed_material == 202)
		fragment_colour.rgb = vec3(1, 0, 1);	///DRAPES
	else if(fixed_material == 203)
		fragment_colour.rgb = vec3(1, 0, 0);	///POLES
	else if(fixed_material == 204)
		fragment_colour.rgb = vec3(0, 1, 1);	///BUDAH
	else if(fixed_material == 205)
		fragment_colour.rgb = vec3(0, 1, 0);	///RABIT
	else if(fixed_material == 206)
		fragment_colour.rgb = vec3(0, 0, 1);	///DRAGON
	else
		fragment_colour.rgb = vec3(0, 0, 0);*/
		
	if(material[fixed_material].mainTexture < 6) {
		fragment_colour = texture(mainTexture[material[fixed_material].mainTexture], varying_texture_coordinate);
		fragment_colour *= texture(normalTexture[material[fixed_material].normalTexture], varying_texture_coordinate) * 0.4f;
	}
	else
		fragment_colour = vec4(material[fixed_material].diffuse, 0);
}