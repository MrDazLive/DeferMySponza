#version 330

struct Light {
	vec3 direction;
	float pad1;
	vec3 intensity;
	float pad2;
};

layout (std140) uniform block_light {
	Light light[2];
};

uniform sampler2DRect colourMap;
uniform sampler2DRect positionMap;
uniform sampler2DRect normalMap;

layout(location = 0)out vec3 fragment_colour;

void main(void) {
   	vec3 Colour = texture(colourMap, gl_FragCoord.xy).xyz;
   	vec3 WorldPos = texture(positionMap, gl_FragCoord.xy).xyz;
   	vec3 Normal = texture(normalMap, gl_FragCoord.xy).xyz;

	fragment_colour = clamp(dot(Normal, normalize(light[0].direction)), 0, 1) * Colour * light[0].intensity;
	fragment_colour += clamp(dot(Normal, normalize(light[1].direction)), 0, 1) * Colour * light[1].intensity;
}