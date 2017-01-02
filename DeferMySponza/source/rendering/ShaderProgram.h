#pragma once

#include <iostream>
#include <GL/glcorearb.h>
#include <functional>

class Shader;
class Texture;
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
	void BindUniformTexture(const Texture *texture, const std::string name, const GLuint offset = 0);
	template <typename ... T> void AddShader(const Shader *shader, const T... arr);
	template <typename ... T> void AddInAttribute(const std::string name, const T... arr);
	template <typename ... T> void AddOutAttribute(const std::string name, const T... arr);
	template <typename T> void BindUniformV3(const T& value, const std::string name);
	template <typename T> void BindUniformM4(const T& value, const std::string name);
#pragma endregion
private:
	GLuint m_id;
	GLuint m_inAttributeCount{ 0 };
	GLuint m_outAttributeCount{ 0 };
	GLint m_status{ GL_FALSE };
};

#pragma region Non-Static Methods

template <typename ... T>
void ShaderProgram::AddShader(const Shader *shader, const T... arr) {
	AddShader(shader);
	AddShader(arr...);
}

template <typename ... T>
void ShaderProgram::AddInAttribute(const std::string name, const T... arr) {
	AddInAttribute(name);
	AddInAttribute(arr...);
}

template <typename ... T>
void ShaderProgram::AddOutAttribute(const std::string name, const T... arr) {
	AddOutAttribute(name);
	AddOutAttribute(arr...);
}

template <typename T>
void ShaderProgram::BindUniformV3(const T& value, const std::string name) {
	GLint id = glGetUniformLocation(m_id, name.c_str());
	glUniform3f(id, value.x, value.y, value.z);
}

template <typename T>
void ShaderProgram::BindUniformM4(const T& value, const std::string name) {
	GLint id = glGetUniformLocation(m_id, name.c_str());
	glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(value));
}

#pragma endregion