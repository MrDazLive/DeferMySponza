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
	if(material[fixed_material].mainTexture < 6) {
		vec4 mT = texture(mainTexture[material[fixed_material].mainTexture], varying_texture_coordinate);
		vec4 nT = texture(normalTexture[material[fixed_material].normalTexture], varying_texture_coordinate);


		fragment_colour = mT;
	}
	else
		fragment_colour = vec4(material[fixed_material].diffuse, 0);
}