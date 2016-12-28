#pragma once

#include <GL/glcorearb.h>

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
	static void Reset();
	static void SetActive(const FrameBufferObject *fbo);
	static void SetWrite(const FrameBufferObject *fbo);
	static void SetRead(const FrameBufferObject *fbo);
#pragma endregion
#pragma region Non-Static Methods
	void SetActive();
	void SetWrite();
	void SetRead();
#pragma endregion
private:
	GLuint m_id{ 0 };
};