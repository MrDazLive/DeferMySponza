#pragma once

#include <string>
#include <GL/glcorearb.h>

class Texture {
public:
#pragma region Constructors/Destructors
	Texture(GLenum target, GLenum attachment = GL_COLOR_ATTACHMENT0);
	~Texture();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
	GLenum getTarget() const;
	GLenum getAttachment() const;
#pragma endregion
#pragma region Static Methods
	static void Reset(const GLenum target);
	static void SetActive(const Texture *vbo);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	void Buffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data = nullptr);
	void LoadFile(const std::string &name);
#pragma endregion
private:
	GLuint m_id;
	GLenum m_target;
	GLenum m_attachment;
};