#include "FrameBufferObject.h"

#include <tgl\tgl.h>
#include <iostream>

#include "Texture.h"

#pragma region Constructors/Destructors

FrameBufferObject::FrameBufferObject() {
	glGenFramebuffers(1, &m_id);
}

FrameBufferObject::~FrameBufferObject() {
	glDeleteFramebuffers(1, &m_id);
}

#pragma endregion
#pragma region Getters/Setters

GLuint FrameBufferObject::getID() const {
	return m_id;
}

#pragma endregion
#pragma region Static Methods

void FrameBufferObject::Reset(GLenum target) {
	glBindFramebuffer(target, 0);
}

void FrameBufferObject::SetActive(const FrameBufferObject *fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->getID());
}

void FrameBufferObject::SetDraw(const FrameBufferObject *fbo) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getID());
}

void FrameBufferObject::SetRead(const FrameBufferObject *fbo) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getID());
}

void FrameBufferObject::LogInfo(const FrameBufferObject *fbo) {
	FrameBufferObject::SetActive(fbo);

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	std::cerr << "FrameBuffer Status: 0x" << std::hex << status << std::endl;

	FrameBufferObject::Reset();
}

#pragma endregion
#pragma region Non-Static Methods

void FrameBufferObject::SetActive() {
	FrameBufferObject::SetActive(this);
}

void FrameBufferObject::SetDraw() {
	FrameBufferObject::SetDraw(this);
}

void FrameBufferObject::SetRead() {
	FrameBufferObject::SetRead(this);
}

void FrameBufferObject::LogInfo() {
	FrameBufferObject::LogInfo(this);
}

void FrameBufferObject::BlitTexture(const Texture *texture, const GLuint width, const GLuint height, GLuint target) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target);
	this->SetRead();

	glReadBuffer(texture->getAttachment());
	glBlitFramebuffer(0, 0, width, height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_LINEAR);

	FrameBufferObject::Reset();
}

void FrameBufferObject::AttachTexture(const Texture *texture, bool drawBuffer) {
	this->SetDraw();
	glFramebufferTexture2D(GL_FRAMEBUFFER, texture->getAttachment(), texture->getTarget(), texture->getID(), 0);
	if (drawBuffer) {
		m_drawBuffer.push_back(texture->getAttachment());
		glDrawBuffers(m_drawBuffer.size(), m_drawBuffer.data());
	}
	FrameBufferObject::Reset(GL_DRAW_FRAMEBUFFER);
}

#pragma endregion