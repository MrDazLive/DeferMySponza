#pragma once

#include <GL/glcorearb.h>

class VertexBufferObject {
public:
#pragma region Constructors/Destructors
	VertexBufferObject(GLenum target, GLenum usage);
	~VertexBufferObject();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
	GLenum getTarget() const;
	GLenum getUsage() const;
#pragma endregion
#pragma region Static Methods
	static void Reset(const GLenum target);
	static void SetActive(const VertexBufferObject *vbo);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	template <typename T> void BufferData(const T &data);
	template <typename T> void BufferData(const T &data, GLuint count);
	template <typename T> void BufferSubData(const T &data, GLintptr offset = 0);
	void BindRange(GLuint count, GLintptr offset = 0, GLuint index = 0);
#pragma endregion
private:
	GLuint m_id;
	GLenum m_target;
	GLenum m_usage;
};

#pragma region Non-Static Methods

template <typename T>
void VertexBufferObject::BufferData(const T &data) {
	this->SetActive();
	glBufferData(m_target,
		data.size() * sizeof(data[0]),
		data.data(),
		m_usage);
	VertexBufferObject::Reset(m_target);
}

template <typename T>
void VertexBufferObject::BufferData(const T &data, GLuint count) {
	this->SetActive();
	glBufferData(m_target,
		count * sizeof(T),
		&data,
		m_usage);
	VertexBufferObject::Reset(m_target);
}

template <typename T>
void VertexBufferObject::BufferSubData(const T &data, GLintptr offset) {
	this->SetActive();
	glBufferSubData(m_target,
		offset,
		data.size() * sizeof(data[0]),
		data.data());
	VertexBufferObject::Reset(m_target);
}

#pragma endregion