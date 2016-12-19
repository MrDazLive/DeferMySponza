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

void VertexArrayObject::Reset() {
	glBindVertexArray(0);
}

void VertexArrayObject::SetActive(const VertexArrayObject *vao) {
	glBindVertexArray(vao->getID());
}

#pragma endregion
#pragma region Non-Static Methods

void VertexArrayObject::SetActive() {
	glBindVertexArray(this->getID());
}

#pragma endregion