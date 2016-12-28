#include "FrameBufferObject.h"

#include <tgl\tgl.h>

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

void FrameBufferObject::Reset() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
}

void FrameBufferObject::SetActive(const FrameBufferObject *fbo) {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo->getID());
}

void FrameBufferObject::SetWrite(const FrameBufferObject *fbo) {
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo->getID());
}

void FrameBufferObject::SetRead(const FrameBufferObject *fbo) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo->getID());
}

#pragma endregion
#pragma region Non-Static Methods

void FrameBufferObject::SetActive() {
	FrameBufferObject::SetActive(this);
}

void FrameBufferObject::SetWrite() {
	FrameBufferObject::SetWrite(this);
}

void FrameBufferObject::SetRead() {
	FrameBufferObject::SetRead(this);
}

#pragma endregion