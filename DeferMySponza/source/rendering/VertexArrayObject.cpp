#include "VertexArrayObject.h"

#include <tgl\tgl.h>

#pragma region Constructors/Destructors

VertexArrayObject::VertexArrayObject() {
	glGenVertexArrays(1, &m_id);
}

VertexArrayObject::~VertexArrayObject() {
	glDeleteVertexArrays(1, &m_id);
}

#pragma endregion
#pragma region Getters/Setters

GLuint VertexArrayObject::getID() const {
	return m_id;
}

GLuint VertexArrayObject::getAttributeCount() const {
	return m_attributeCount;
}

#pragma endregion
#pragma region Static Methods

void VertexArrayObject::SetActive(const VertexArrayObject *vao) {
	glBindVertexArray((vao != nullptr) ? vao->getID() : 0);
}

#pragma endregion
#pragma region Non-Static Methods

void VertexArrayObject::SetActive() {
	glBindVertexArray(this->getID());
}

template <typename T>
void VertexArrayObject::AddVertexAttribute(GLint size, GLenum type, GLboolean normalized, const GLvoid *pointer = 0) {
	glEnableVertexAttribArray(this->getAttributeCount());
	glVertexAttribPointer(this->getAttributeCount(), size, type, normalized, sizeof(T), *pointer);
	m_attributeCount++;
}

void VertexArrayObject::BindBuffer(GLenum target, GLuint buffer) {
	glBindBuffer(target, buffer);
}

#pragma endregion