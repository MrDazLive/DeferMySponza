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

uniform	sampler2D gColourMap;
uniform	sampler2D gPositionMap;
uniform	sampler2D gNormalMap;

layout(location = 0)out vec3 fragment_colour;

void main(void) {
   	vec3 Colour = texture(gColourMap, gl_FragCoord.xy).xyz;
   	vec3 WorldPos = texture(gPositionMap, gl_FragCoord.xy).xyz;
   	vec3 Normal = texture(gNormalMap, gl_FragCoord.xy).xyz;
   	Normal = normalize(Normal);
	
	fragment_colour = vec3(0);

	/*fragment_colour = clamp(dot(Normal, normalize(light[0].direction)), 0, 1) * Colour * light[0].intensity;
	fragment_colour += clamp(dot(Normal, normalize(light[1].direction)), 0, 1) * Colour * light[1].intensity;
	fragment_colour = light[0].direction;*/
}