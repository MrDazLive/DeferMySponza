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
	static void SetActive(const VertexArrayObject *vao);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	template <typename T> void AddVertexAttribute(GLint size, GLenum type, GLboolean normalized, const GLvoid *pointer = 0);
#pragma endregion
private:
	GLuint m_id;
	GLuint m_attributeCount = 0;
};