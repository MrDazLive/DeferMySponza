#version 330

struct Material {
	vec3 diffuse;
	float shine;
	vec3 specular;
	float metallic;

	int mainTexture;
	int normalTexture;
	int excess1;
	int excess2;
};

layout (std140) uniform block_material {
	Material material[7];
};

struct Light {
	vec3 position;
	vec3 direction;
	vec3 intensity;
	float range;
	float coneAngle;
};

uniform vec3 eyePosition;
uniform vec3 ambience;

uniform sampler2DRect colourMap;
uniform sampler2DRect positionMap;
uniform sampler2DRect normalMap;
uniform sampler2DRect materialMap;

flat in Light fixed_light;

layout(location = 0)out vec3 fragment_colour;

vec3 Colour;
vec3 WorldPosition;
vec3 Normal;
vec2 TextureCoordinate;
int MaterialID;

void getMetallic(inout float diffuse, inout float specular) {
	float met = material[MaterialID].metallic;
	specular = mix(specular, 1, met);
	diffuse *= (1 - met);
}

vec3 getInternal(vec3 lightDirection, vec3 lightIntensity) {
	float diffuse = clamp(dot(Normal, lightDirection), 0, 1);
	if(diffuse > 0) {
		vec3 dir = normalize(eyePosition - WorldPosition);
		vec3 ref = normalize(reflect(lightDirection, Normal));

		float specular = clamp(dot(dir, ref), 0, 1);
		
		getMetallic(diffuse, specular);

		return vec3(diffuse + specular) * Colour * lightIntensity;
	}
	return vec3(0);
}

vec3 getPoint(Light l) {
	vec3 dir = l.position - WorldPosition;
	float dis = length(dir);
	dir = normalize(dir);

	float att = l.range / (dis * dis);

	return att * getInternal(dir, l.intensity);
}

void main(void) {
   	Colour = texture(colourMap, gl_FragCoord.xy).xyz;
   	WorldPosition = texture(positionMap, gl_FragCoord.xy).xyz;
   	Normal = texture(normalMap, gl_FragCoord.xy).xyz;
	TextureCoordinate = texture(materialMap, gl_FragCoord.xy).xy;
	MaterialID = int(texture(materialMap, gl_FragCoord.xy).z);
	
	fragment_colour = getPoint(fixed_light) / ambience;
}