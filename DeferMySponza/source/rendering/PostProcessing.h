#pragma once

#include <string>
#include <GL/glcorearb.h>

class VertexBufferObject;
class VertexArrayObject;
class FrameBufferObject;
class ShaderProgram;
class Texture;
class Shader;

class PostProcessing {
public:
#pragma region Constructors/Destructors
	PostProcessing(std::string vs, std::string fs);
	~PostProcessing();
#pragma endregion
#pragma region Getters/Setters
	void setSourceTexture(Texture *texture);
	void setTextureSize(const GLsizei width, const GLsizei height);
#pragma endregion
#pragma region Static Methods

#pragma endregion
#pragma region Non-Static Methods
	void Draw();
#pragma endregion
private:
	VertexBufferObject *m_vbo{ nullptr };
	VertexArrayObject *m_vao{ nullptr };
	FrameBufferObject *m_fbo{ nullptr };
	ShaderProgram *m_pro{ nullptr };
	Texture *m_tex{ nullptr };
	Shader *m_vs{ nullptr };
	Shader *m_fs{ nullptr };

	GLsizei m_width{ 0 };
	GLsizei m_height{ 0 };
};