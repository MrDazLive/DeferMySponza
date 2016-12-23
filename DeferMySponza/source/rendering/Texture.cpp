#include "Texture.h"

#include <iostream>
#include <tgl\tgl.h>
#include <tygra\FileHelper.hpp>

#pragma region Constructors/Destructors

Texture::Texture(GLenum target) {
	glGenTextures(1, &m_id);
	m_target = target;
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
		glTexImage2D(m_target,
			0,
			GL_RGBA,
			image.width(),
			image.height(),
			0,
			pixel_formats[image.componentsPerPixel()],
			image.bytesPerComponent() == 1 ?
			GL_UNSIGNED_BYTE :
			GL_UNSIGNED_SHORT,
			image.pixelData());
		glGenerateMipmap(m_target);
		glBindTexture(m_target, 0);

		Texture::Reset(m_target);
	} else {
		std::cerr << name << " failed to load." << std::endl;
	}

}

#pragma endregion