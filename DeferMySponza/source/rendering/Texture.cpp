#include "Texture.h"

#include <iostream>
#include <tgl\tgl.h>
#include <tygra\FileHelper.hpp>

#pragma region Constructors/Destructors

Texture::Texture(GLenum target, GLenum attachment) {
	glGenTextures(1, &m_id);
	m_target = target;
	m_attachment = attachment;
}

Texture::~Texture() {
	glDeleteTextures(1, &m_id);
}

#pragma endregion
#pragma region Getters/Setters

GLuint Texture::getID() const {
	return m_id;
}

GLenum Texture::getTarget() const {
	return m_target;
}

GLenum Texture::getAttachment() const {
	return m_attachment;
}

#pragma endregion
#pragma region Static Methods

void Texture::Reset(const GLenum target) {
	glBindTexture(target, 0);
}

void Texture::SetActive(const Texture *vbo) {
	glBindTexture(vbo->getTarget(), vbo->getID());
}

#pragma endregion
#pragma region Non-Static Methods

void Texture::SetActive() {
	Texture::SetActive(this);
}

void Texture::Buffer(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *data) {
	this->SetActive();
	glTexImage2D(m_target, 0, internalFormat, width, height, 0, format, type, data);
	Texture::Reset(m_target);
}

void Texture::LoadFile(const std::string &name) {
	tygra::Image image = tygra::createImageFromPngFile(""+name);
	if (image.doesContainData()) {
		this->SetActive();

		glTexParameteri(m_target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(m_target, GL_TEXTURE_MIN_FILTER,
			GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(m_target, GL_TEXTURE_WRAP_T, GL_REPEAT);

		GLenum pixel_formats[] = { 0, GL_RED, GL_RG, GL_RGB, GL_RGBA };
		this->Buffer(GL_RGBA,
			image.width(),
			image.height(),
			pixel_formats[image.componentsPerPixel()],
			image.bytesPerComponent() == 1 ?
			GL_UNSIGNED_BYTE :
			GL_UNSIGNED_SHORT,
			image.pixelData());
		this->SetActive();
		glGenerateMipmap(m_target);
		glBindTexture(m_target, 0);

		Texture::Reset(m_target);
	} else {
		std::cerr << name << " failed to load." << std::endl;
	}

}

#pragma endregion