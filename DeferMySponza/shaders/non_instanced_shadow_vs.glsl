#version 330

struct Instance {
	mat4 transform;
	int material;
};

uniform mat4 combined_transform;
uniform Instance model;

layout(location = 0) in vec3 vertex_position;

void main(void) {
	gl_Position = combined_transform * model.transform * vec4(vertex_position, 1.0);
}