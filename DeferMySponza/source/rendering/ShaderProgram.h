#pragma once

#include<iostream>
#include <GL/glcorearb.h>
#include <functional>

class Shader;
template <typename T> class VertexBufferObject;

class ShaderProgram {
public:
#pragma region Constructors/Destructors
	ShaderProgram();
	~ShaderProgram();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
	GLuint getInAttributeCount() const;
	GLuint getOutAttributeCount() const;
#pragma endregion
#pragma region Static Methods
	static void Reset();
	static void SetActive(const ShaderProgram *program);
#pragma endregion
#pragma region Non-Static Methods
	void Link();
	void SetActive();
	void AddShader(const Shader *shader);
	void AddInAttribute(const std::string name);
	void AddOutAttribute(const std::string name);
	template <typename T> void BindBlock(const VertexBufferObject<T> *vbo, const std::string name);
	template <typename T> void BindUniform(std::function<void(GLint, GLsizei, GLboolean, const GLfloat *)> func, const T& value, const std::string name);
#pragma endregion
private:
	GLuint m_id;
	GLuint m_inAttributeCount{ 0 };
	GLuint m_outAttributeCount{ 0 };
};

#pragma region Non-Static Methods

template <typename T>
void ShaderProgram::BindBlock(const VertexBufferObject<T> *vbo, const std::string name) {
	vbo->SetActive();
	GLuint index = glGetUniformBlockIndex(m_id, name);
	glUniformBlockBinding(m_id, index, 0);
	VertexBuffer::Reset();
}

template <typename T>
void ShaderProgram::BindUniform(std::function<void(GLint, GLsizei, GLboolean, const GLfloat *)> func, const T& value, const std::string name) {
	GLint id = glGetUniformLocation(m_id, name.c_str());
	func(id, 1, GL_FALSE, glm::value_ptr(value));
}

#pragma endregion