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
uniform	mat4 shadowTransform[5];

flat out int fixed_instance;
flat out Light fixed_light;

mat4 lookAt(vec3 eye, vec3 center, vec3 up) {
	vec3 z = normalize(center - eye);
	vec3 x = normalize(cross(up, z));
	vec3 y = cross(z, x);

	return mat4(vec4(x, 0), vec4(y, 0), vec4(z, 0), vec4(0, 0, 0, 1));
}

void main(void) {
	float rad = tan(light.coneAngle / 2) * light.range;
	float ran = light.range;
	vec3 pos = light.position;
	mat4 model = mat4(vec4(rad, 0, 0, 0), vec4(0, rad, 0, 0), vec4(0, 0, ran, 0), vec4(pos, 1));
	mat4 view = lookAt(vec3(0), -light.direction, vec3(0, 1, 0));

	gl_Position = combined_transform * model * view * vec4(vertex_coord, 1.0);
	fixed_light = light;
	fixed_light.direction = normalize(light.direction);

	fixed_instance = gl_InstanceID;
}