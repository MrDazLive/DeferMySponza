#pragma once

#include <iostream>
#include <GL/glcorearb.h>
#include <functional>

class Shader;
class VertexBufferObject;

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
	GLint getStatus() const;
#pragma endregion
#pragma region Static Methods
	static void Reset();
	static void SetActive(const ShaderProgram *program);
	static void LogInfo(const ShaderProgram *shader);
#pragma endregion
#pragma region Non-Static Methods
	void Link();
	void SetActive();
	void LogInfo();
	void AddShader(const Shader *shader);
	void AddInAttribute(const std::string name);
	void AddOutAttribute(const std::string name);
	void BindBlock(VertexBufferObject *vbo, const std::string name);
	template <typename T> void BindUniform(std::function<void(GLint, GLsizei, GLboolean, const GLfloat *)> func, const T& value, const std::string name);
#pragma endregion
private:
	GLuint m_id;
	GLuint m_inAttributeCount{ 0 };
	GLuint m_outAttributeCount{ 0 };
	GLint m_status{ GL_FALSE };
};

#pragma region Non-Static Methods

template <typename T>
void ShaderProgram::BindUniform(std::function<void(GLint, GLsizei, GLboolean, const GLfloat *)> func, const T& value, const std::string name) {
	GLint id = glGetUniformLocation(m_id, name.c_str());
	func(id, 1, GL_FALSE, glm::value_ptr(value));
}

#pragma endregion