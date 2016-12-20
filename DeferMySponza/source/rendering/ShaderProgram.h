#pragma once

#include<iostream>
#include <GL/glcorearb.h>

class Shader;

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
#pragma endregion
private:
	GLuint m_id;
	GLuint m_inAttributeCount{ 0 };
	GLuint m_outAttributeCount{ 0 };
};