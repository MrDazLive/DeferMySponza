#include "VertexBufferObject.h"

#include <tgl\tgl.h>

#pragma region Constructors/Destructors

VertexBufferObject::VertexBufferObject(GLenum target, GLenum usage) {
	glGenBuffers(1, &m_id);
	m_target = target;
	m_usage = usage;
}

VertexBufferObject::~VertexBufferObject() {
	glDeleteBuffers(1, &m_id);
}

#pragma endregion
#pragma region Getters/Setters

GLuint VertexBufferObject::getID() const {
	return m_id;
}

GLenum VertexBufferObject::getTarget() const {
	return m_target;
}

GLenum VertexBufferObject::getUsage() const {
	return m_usage;
}

#pragma endregion
#pragma region Static Methods

void VertexBufferObject::Reset(const GLenum target) {
	glBindBuffer(target, 0);
}

void VertexBufferObject::SetActive(const VertexBufferObject *vbo) {
	glBindBuffer(vbo->getTarget(), vbo->getID());
}

#pragma endregion
#pragma region Non-Static Methods

void VertexBufferObject::SetActive() {
	VertexBufferObject::SetActive(this);
}

void VertexBufferObject::BindRange(GLuint index, GLintptr offset, GLuint size) {
	this->SetActive();
	glBindBufferRange(m_usage,
		index,
		m_id,
		offset,
		size);
	VertexBufferObject::Reset(m_target);
}

#pragma endregion