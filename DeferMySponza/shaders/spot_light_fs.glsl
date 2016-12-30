#version 330

struct Light {
	vec3 position;
	float range;
	vec3 direction;
	float coneAngle;
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

vec3 getSpot(Light l) {
	vec3 dir = normalize(WorldPosition - l.position);
	float dis = length(dir);
	dir = normalize(dir);

	float fac = dot(l.direction, dir);
	float ang = cos(l.coneAngle / 2);

	if(fac > ang) {
		float dAtt = l.range / (dis * dis);
		float cAtt = 1 - ((1 - fac) / (1 - ang));

		return dAtt * cAtt * getInternal(dir, l.intensity);
	}
	return vec3(0);
}

void main(void) {
   	Colour = texture(colourMap, gl_FragCoord.xy).xyz;
   	WorldPosition = texture(positionMap, gl_FragCoord.xy).xyz;
   	Normal = texture(normalMap, gl_FragCoord.xy).xyz;
	
	fragment_colour = vec3(0);
	for(int i = 0; i < 3; i++) {
		fragment_colour += getSpot(light[i]);
	}
}