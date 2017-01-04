#version 330

struct Light {
	vec3 position;
	vec3 direction;
	vec3 intensity;
	float range;
	float coneAngle;
};

layout(location = 0) in vec3 vertex_coord;
layout(location = 1) in Light light;

uniform mat4 combined_transform;

flat out Light fixed_light;

void main(void) {
	float rad = light.coneAngle;
	float ran = light.range;
	vec3 dir = light.direction;
	vec3 pos = light.position;
	mat4 model = mat4(vec4(rad, 0, 0, 0), vec4(0, rad, 0, 0), vec4(0, 0, ran, 0), vec4(pos, 1));

	gl_Position = combined_transform * model * vec4(vertex_coord, 1.0);
	fixed_light = light;
	fixed_light.direction = normalize(fixed_light.direction);
}