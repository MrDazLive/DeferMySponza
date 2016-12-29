#version 330

struct Instance {
	mat4 transform;
	int material;
};

uniform mat4 combined_transform;
uniform Instance model;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec3 vertex_tangent;
layout(location = 3) in vec2 vertex_texture_coordinate;

out vec3 varying_position;
out vec3 varying_normal;
out vec3 varying_tangent;
out vec2 varying_texture_coordinate;

flat out int fixed_material;

void main(void) {
	gl_Position = combined_transform * model.transform * vec4(vertex_position, 1.0);
	varying_position = mat4x3(model.transform) * vec4(vertex_position, 1.0);
	varying_normal = mat3(model.transform) * vertex_normal;
	varying_tangent = mat3(model.transform) * vertex_tangent;
	varying_texture_coordinate = vertex_texture_coordinate;

	fixed_material = model.material - 200;
}