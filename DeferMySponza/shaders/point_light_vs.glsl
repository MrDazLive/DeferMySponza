#version 330

struct Light {
	vec3 position;
	vec3 intensity;
	float range;
};

layout(location = 0) in vec3 vertex_coord;
layout(location = 1) in Light light;

uniform mat4 combined_transform;

flat out Light fixed_light;

void main(void) {
	float r = light.range * 2;
	vec3 pos = light.position;
	mat4 model = mat4( vec4( r, 0, 0, 0), vec4(0, r, 0, 0), vec4(0, 0, r, 0), vec4(pos, 1));

    gl_Position = combined_transform * model * vec4(vertex_coord, 1.0);
	fixed_light = light;
}