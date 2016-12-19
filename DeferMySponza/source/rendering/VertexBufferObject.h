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

	void setData(const T *data);
#pragma endregion
#pragma region Static Methods
	static void SetActive(const GLenum target);
	static void SetActive(const VertexBufferObject *vbo);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	void BufferData();
#pragma endregion
private:
	GLuint m_id;
	GLenum m_target;
	GLenum m_usage;
	T *m_data;
};