#pragma once

#include <GL/glcorearb.h>

template <typename T>
class VertexBufferObject {
public:
#pragma region Constructors/Destructors
	VertexBufferObject<T>(GLenum target, GLenum usage);
	~VertexBufferObject();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
	GLuint getTarget() const;
	GLuint getUsage() const;

	void setData(T *data);
	void setSize(GLuint size);
#pragma endregion
#pragma region Static Methods
	static void Reset(const GLenum target);
	static void SetActive(const VertexBufferObject *vbo);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	void BufferData();
	void BufferSubData(GLintptr offset = 0);
	void BindRange(GLuint index = 0, GLintptr offset = 0);
#pragma endregion
private:
	GLuint m_id;
	GLenum m_target;
	GLenum m_usage;
	T *m_data;
	GLuint m_size;
};

#define VertexBuffer VertexBufferObject<void>

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
	return m_target;
}

template <typename T>
GLuint VertexBufferObject<T>::getUsage() const {
	return m_usage;
}

template <typename T>
void VertexBufferObject<T>::setData(T *data) {
	m_data = data;
}

template <typename T>
void VertexBufferObject<T>::setSize(GLuint size) {
	m_size = size;
}

#pragma endregion
#pragma region Static Methods

template <typename T>
void VertexBufferObject<T>::Reset(const GLenum target) {
	glBindBuffer(target, 0);
}

template <typename T>
void VertexBufferObject<T>::SetActive(const VertexBufferObject *vbo) {
	glBindBuffer(vbo->getTarget(), vbo->getID());
}

#pragma endregion
#pragma region Non-Static Methods

template <typename T>
void VertexBufferObject<T>::SetActive() {
	glBindBuffer(this->getTarget(), this->getID());
}

template <typename T>
void VertexBufferObject<T>::BufferData() {
	this->SetActive();
	glBufferData(m_target,
		m_size * sizeof(T),
		m_data,
		m_usage);
	VertexBufferObject::Reset(m_target);
}

template <typename T>
void VertexBufferObject<T>::BufferSubData(GLintptr offset) {
	this->SetActive();
	glBufferSubData(m_target,
		offset,
		*m_data.size() * sizeof(T),
		*m_data.data());
	VertexBufferObject::Reset(m_target);
}

template <typename T>
void VertexBufferObject<T>::BindRange(GLuint index, GLintptr offset) {
	this->SetActive();
	glBindBufferRange(m_usage,
		index,
		m_id,
		offset,
		*m_data.size() * sizeof(T));
	VertexBufferObject::Reset(m_target);
}

#pragma endregion