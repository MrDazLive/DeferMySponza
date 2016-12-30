#version 330

struct Light {
	vec3 position;
	float range;
	vec3 intensity;
	float pad1;
};

layout (std140) uniform block_light {
	Light light[20];
};

uniform vec3 eyePosition;

uniform sampler2DRect colourMap;
uniform sampler2DRect positionMap;
uniform sampler2DRect normalMap;

layout(location = 0)out vec3 fragment_colour;

vec3 Colour;
vec3 WorldPosition;
vec3 Normal;

vec3 getInternal(vec3 lightDirection, vec3 lightIntensity) {
	float diffuse = clamp(dot(Normal, normalize(lightDirection)), 0, 1);
	if(diffuse > 0) {
		vec3 dir = normalize(eyePosition - WorldPosition);
		vec3 ref = normalize(reflect(lightDirection, Normal));

		float specular = clamp(dot(dir, ref), 0, 1);
		return vec3(diffuse + specular) * Colour * lightIntensity;
	}
	return vec3(diffuse) * Colour * lightIntensity;
}

vec3 getPoint(Light l) {
	vec3 dir = WorldPosition - l.position;
	float dis = length(dir);
	dir = normalize(dir);

	float fac = 1 / l.range;
	float att = 1 / (dis * dis * fac);

	return att * getInternal(dir, l.intensity);
}

void main(void) {
   	Colour = texture(colourMap, gl_FragCoord.xy).xyz;
   	WorldPosition = texture(positionMap, gl_FragCoord.xy).xyz;
   	Normal = texture(normalMap, gl_FragCoord.xy).xyz;
	
	fragment_colour = getPoint(light[0]);
	for(int i = 1; i < 20; i++) {
		fragment_colour += getPoint(light[i]);
	}
}