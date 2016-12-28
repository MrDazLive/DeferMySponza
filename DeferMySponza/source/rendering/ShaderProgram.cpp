#include "ShaderProgram.h"

#include <tgl\tgl.h>
#include "Shader.h"
#include "Texture.h"
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

GLint ShaderProgram::getStatus() const {
	return m_status;
}

#pragma endregion
#pragma region Static Methods

void ShaderProgram::Reset() {
	glLinkProgram(0);
}

void ShaderProgram::SetActive(const ShaderProgram *program) {
	glUseProgram(program->getID());
}

void ShaderProgram::LogInfo(const ShaderProgram *shader) {
	const int string_length = 1024;
	GLchar log[string_length] = "";
	glGetProgramInfoLog(shader->getID(), string_length, NULL, log);
	std::cerr << log << std::endl;
}

#pragma endregion
#pragma region Non-Static Methods

void ShaderProgram::Link() {
	glLinkProgram(this->getID());
	glGetProgramiv(this->getID(), GL_LINK_STATUS, &m_status);
	if (m_status != GL_TRUE) {
		this->LogInfo();
	}
}

void ShaderProgram::SetActive() {
	ShaderProgram::SetActive(this);
}

void ShaderProgram::LogInfo() {
	ShaderProgram::LogInfo(this);
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

void ShaderProgram::BindBlock(VertexBufferObject *vbo, const std::string name) {
	vbo->SetActive();
	GLuint index = glGetUniformBlockIndex(m_id, name.c_str());
	glUniformBlockBinding(m_id, index, 0);
	VertexBufferObject::Reset(vbo->getTarget());
}

void ShaderProgram::BindUniformTexture(const Texture *texture, const std::string name) {
	glActiveTexture(GL_TEXTURE0 + m_textureCount);
	glBindTexture(texture->getTarget(), texture->getID());
	GLuint id = glGetUniformLocation(this->getID(), name.c_str());
	glUniform1i(id, m_textureCount);

	m_textureCount++;
}

#pragma endregion