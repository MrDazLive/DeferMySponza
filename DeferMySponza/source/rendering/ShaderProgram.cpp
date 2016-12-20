#include "ShaderProgram.h"

#include <tgl\tgl.h>
#include "Shader.h"
#include "VertexBufferObject.h"

#pragma region Constructors/Destructors

ShaderProgram::ShaderProgram() {
	m_id = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
	glDeleteProgram(m_id);
}

#pragma endregion
#pragma region Getters/Setters
GLuint ShaderProgram::getID() const {
	return m_id;
}

GLuint ShaderProgram::getInAttributeCount() const {
	return m_inAttributeCount;
}

GLuint ShaderProgram::getOutAttributeCount() const {
	return m_outAttributeCount;
}

#pragma endregion
#pragma region Static Methods

void ShaderProgram::Reset() {
	glLinkProgram(0);
}

void ShaderProgram::SetActive(const ShaderProgram *program) {
	glUseProgram(program->getID());
}

#pragma endregion
#pragma region Non-Static Methods

void ShaderProgram::Link() {
	glLinkProgram(this->getID());
}

void ShaderProgram::SetActive() {
	ShaderProgram::SetActive(this);
}

void ShaderProgram::AddShader(const Shader *shader) {
	glAttachShader(this->getID(), shader->getID());
}

void ShaderProgram::AddInAttribute(const std::string name) {
	glBindAttribLocation(this->getID(), this->getInAttributeCount(), name.c_str());
	m_inAttributeCount++;
}

void ShaderProgram::AddOutAttribute(const std::string name) {
	glBindAttribLocation(this->getID(), this->getOutAttributeCount(), name.c_str());
	m_outAttributeCount++;
}

#pragma endregion