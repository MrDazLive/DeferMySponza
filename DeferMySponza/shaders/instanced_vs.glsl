#version 330

uniform mat4 combined_xform;

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_texture_coordinate;

layout(location = 3) in mat4 model_transform;

out vec3 varying_position;
out vec3 varying_normal;
out vec2 varying_texture_coordinate;

void main(void) {
	gl_Position = combined_xform * model_transform * vec4(vertex_position, 1.0);
	varying_position = mat4x3(model_transform) * vec4(vertex_position, 1.0);
	varying_normal = mat3(model_transform) * vertex_normal;
	varying_texture_coordinate = vertex_texture_coordinate;
}