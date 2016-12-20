#pragma once

#include <GL/glcorearb.h>

class VertexArrayObject {
public:
#pragma region Constructors/Destructors
	VertexArrayObject();
	~VertexArrayObject();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
	GLuint getAttributeCount() const;
#pragma endregion
#pragma region Static Methods
	static void Reset();
	static void SetActive(const VertexArrayObject *vao);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	template <typename T> void AddAttribute(GLint size, GLenum type, GLboolean normalized, const GLvoid *pointer = 0);
	template <typename T> void AddAttributeDivisor(GLint size, GLenum type, GLboolean normalized, const GLvoid *pointer = 0);
#pragma endregion
private:
	GLuint m_id{ 0 };
	GLuint m_attributeCount{ 0 };
};

#pragma region Template Methods

template <typename T> void VertexArrayObject::AddAttribute(GLint size, GLenum type, GLboolean normalized, const GLvoid *pointer) {
	glEnableVertexAttribArray(this->getAttributeCount());
	glVertexAttribPointer(this->getAttributeCount(), size, type, normalized, sizeof(T), pointer);
	m_attributeCount++;
}

template <typename T> void VertexArrayObject::AddAttributeDivisor(GLint size, GLenum type, GLboolean normalized, const GLvoid *pointer) {
	glEnableVertexAttribArray(this->getAttributeCount());
	glVertexAttribPointer(this->getAttributeCount(), size, type, normalized, sizeof(T), pointer);
	glVertexAttribDivisor(this->getAttributeCount(), 1);
	m_attributeCount++;
}

#pragma endregion