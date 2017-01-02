#version 330

struct Light {
	vec3 position;
	vec3 direction;
	vec3 intensity;
	float range;
	float coneAngle;
};

layout(location = 0) in vec2 vertex_coord;
layout(location = 1) in Light light;

flat out Light fixed_light;

void main(void) {
    gl_Position = vec4(vertex_coord, 0.0, 1.0);
	fixed_light = light;
	fixed_light.direction = normalize(fixed_light.direction);
}