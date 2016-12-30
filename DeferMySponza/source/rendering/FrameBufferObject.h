#pragma once

#include <vector>
#include <GL/glcorearb.h>

class Texture;

class FrameBufferObject {
public:
#pragma region Constructors/Destructors
	FrameBufferObject();
	~FrameBufferObject();
#pragma endregion
#pragma region Getters/Setters
	GLuint getID() const;
#pragma endregion
#pragma region Static Methods
	static void Reset(GLenum target = GL_FRAMEBUFFER);
	static void SetActive(const FrameBufferObject *fbo);
	static void SetDraw(const FrameBufferObject *fbo);
	static void SetRead(const FrameBufferObject *fbo);
	static void LogInfo(const FrameBufferObject *fbo);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	void SetDraw();
	void SetRead();
	void LogInfo();
	void BlitTexture(const Texture *texture, const GLuint width, const GLuint height, GLuint target = 0);
	void AttachTexture(const Texture *texture, bool drawBuffer = true);
#pragma endregion
private:
	GLuint m_id{ 0 };
	std::vector<GLenum> m_drawBuffer;
};