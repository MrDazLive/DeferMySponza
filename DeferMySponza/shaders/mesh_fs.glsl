#version 330

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

uniform vec3 camera_position;

in vec3 varying_position;
in vec3 varying_normal;
in vec3 varying_tangent;
in vec2 varying_texture_coordinate;

flat in int fixed_material;

out vec4 fragment_colour;

vec3 light_dir = vec3(1, .5f, -1);

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

vec2 parallaxMapping() { 
	vec3 n = normalize(varying_normal);
	vec3 t = normalize(varying_tangent);
	t = normalize(t - dot(t, n) * n);
	vec3 b = cross(t, n);
	mat3 TBN = mat3(t, b, n);

	vec3 tPos = (camera_position);
	vec3 tCam = (varying_position);
	vec3 viewDir = normalize(tCam - tPos);
	viewDir = viewDir * TBN;

	return varying_texture_coordinate +
		(viewDir.xy) * texture(normalTexture[material[fixed_material].normalTexture], varying_texture_coordinate).r * .3f;
}

void main(void) {		
	if(material[fixed_material].mainTexture < 6) {
		vec2 coord = varying_texture_coordinate;// parallaxMapping();
		vec4 mT = texture(mainTexture[material[fixed_material].mainTexture], coord);

		vec3 normal = bumpNormal(coord);
		float i = clamp(dot(normal, normalize(light_dir)), 0, 1);

		fragment_colour = mT * vec4(material[fixed_material].diffuse, 0) * i;
	} else {
		vec3 normal = varying_normal;
		float i = clamp(dot(normal, normalize(light_dir)), 0, 1);

		fragment_colour = vec4(material[fixed_material].diffuse, 0) * i;
	}
}