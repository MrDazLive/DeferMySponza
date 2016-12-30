#version 420

struct Material {
	vec3 diffuse;
	float shine;

	vec3 specular;
	int mainTexture;

	int excess0;
	int excess1;
	int normalTexture;
	int excess2;
};

layout (std140) uniform block_material {
	Material material[7];
};

uniform	sampler2D mainTexture[7];
uniform	sampler2D normalTexture[7];

uniform vec3 ambience;

in vec3 varying_position;
in vec3 varying_normal;
in vec3 varying_tangent;
in vec2 varying_texture_coordinate;

flat in int fixed_material;

layout(location = 0)out vec3 fragment_colour;

vec3 bumpNormal(vec2 coord) {
	vec3 n = normalize(varying_normal);
	vec3 t = normalize(varying_tangent);
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(t, n);

	vec3 bump = texture(normalTexture[material[fixed_material].normalTexture], coord).rgb;
	bump = 2 * bump - vec3(1);
	mat3 TBN = mat3(t, b, n);

	return normalize(TBN * bump);
}

void main(void) {	
	if(material[fixed_material].mainTexture < 6) {
		vec2 coord = varying_texture_coordinate;
		vec3 mT = texture(mainTexture[material[fixed_material].mainTexture], coord).xyz;

		vec3 normal = bumpNormal(coord);

		fragment_colour = mT * material[fixed_material].diffuse * ambience;
	} else {
		vec3 normal = varying_normal;
		
		fragment_colour = material[fixed_material].diffuse * ambience;
	}
}