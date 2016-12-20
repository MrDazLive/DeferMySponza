#pragma once

#include<iostream>
#include <GL/glcorearb.h>

class Shader {
public:
#pragma region Constructors/Destructors
	Shader(GLenum shaderType);
	~Shader();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
	GLenum getShaderType() const;
	GLint getStatus() const;
#pragma endregion
#pragma region Static Methods
	static void LogInfo(const Shader *shader);
#pragma endregion
#pragma region Non-Static Methods
	void LogInfo();
	void LoadFile(const std::string name);
#pragma endregion
private:
	GLuint m_id;
	GLenum m_shaderType;
	GLint m_status{ GL_FALSE };
};