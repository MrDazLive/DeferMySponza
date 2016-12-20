#include "Shader.h"

#include <iostream>
#include <tgl\tgl.h>
#include <tygra\FileHelper.hpp>

#pragma region Constructors/Destructors

Shader::Shader(GLenum shaderType) {
	m_id = glCreateShader(shaderType);
	m_shaderType = shaderType;
}

Shader::~Shader() {
	glDeleteShader(m_id);
}

#pragma endregion
#pragma region Getters/Setters

GLuint Shader::getID() const {
	return m_id;
}

GLenum Shader::getShaderType() const {
	return m_shaderType;
}

GLint Shader::getStatus() const {
	return m_status;
}

#pragma endregion
#pragma region Static Methods

void Shader::LogInfo(const Shader *shader) {
	const int string_length = 1024;
	GLchar log[string_length] = "";
	glGetShaderInfoLog(shader->getID(), string_length, NULL, log);
	std::cerr << log << std::endl;
}

#pragma endregion
#pragma region Non-Static Methods

void Shader::LogInfo() {
	Shader::LogInfo(this);
}

void Shader::LoadFile(const std::string name) {
	std::string shader_string = tygra::createStringFromFile(name);
	const char *shader_code = shader_string.c_str();
	glShaderSource(m_id, 1,
		(const GLchar **)&shader_code, NULL);
	glCompileShader(m_id);
	glGetShaderiv(m_id, GL_COMPILE_STATUS, &m_status);
}

#pragma endregion