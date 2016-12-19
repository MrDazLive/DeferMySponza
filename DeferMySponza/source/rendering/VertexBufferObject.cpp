#include "VertexBufferObject.h"

#include <tgl\tgl.h>

#pragma region Constructors/Destructors

template <typename T>
VertexBufferObject<T>::VertexBufferObject(GLenum target, GLenum usage) {
	glGenBuffers(1, &m_id);
	m_target = target;
	m_usage = usage;
}

template <typename T>
VertexBufferObject<T>::~VertexBufferObject() {
	glDeleteBuffers(1, &m_id);
}

#pragma endregion
#pragma region Getters/Setters

template <typename T>
GLuint VertexBufferObject<T>::getID() const {
	return m_id;
}

template <typename T>
GLuint VertexBufferObject<T>::getTarget() const {
	retrun m_target;
}

template <typename T>
GLuint VertexBufferObject<T>::getUsage() const {
	retrun m_usage;
}

template <typename T>
void VertexBufferObject<T>::setData(const T *data) {
	m_data = data;
}

#pragma endregion
#pragma region Static Methods

template <typename T>
void VertexBufferObject<T>::SetActive(const GLenum target) {
	glBindVertexArray(target, 0);
}

template <typename T>
void VertexBufferObject<T>::SetActive(const VertexBufferObject *vbo) {
	glBindVertexArray(vbo->getTarget(), vbo->getID());
}

#pragma endregion
#pragma region Non-Static Methods

template <typename T>
void VertexBufferObject<T>::SetActive() {
	glBindVertexArray(this->getID());
}

template <typename T>
void VertexBufferObject<T>::BufferData() {
	this->SetActive();
	glBufferData(m_target,
		*m_data.size() * sizeof(T),
		*m_data.data(),
		m_usage);
	VertexBufferObject::SetActive(m_target);
}

#pragma endregion