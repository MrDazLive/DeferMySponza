#include "PostProcessing.h"

#include <glm\vec2.hpp>
#include <tgl\tgl.h>
#include <vector>

#include "VertexBufferObject.h"
#include "VertexArrayObject.h"
#include "FrameBufferObject.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Shader.h"

#pragma region Constructors/Destructors

PostProcessing::PostProcessing(std::string vs, std::string fs) {
	m_vbo = new VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	std::vector<glm::vec2> v = { glm::vec2(-1, -1), 
		glm::vec2(1, -1),
		glm::vec2(1, 1), 
		glm::vec2(-1, 1) };
	m_vbo->BufferData(v);

	m_vao = new VertexArrayObject();
	m_vao->SetActive();
	m_vbo->SetActive();
	m_vao->AddAttribute<glm::vec2>(2, GL_FLOAT, GL_FALSE);
	VertexArrayObject::Reset();
	VertexBufferObject::Reset(GL_ARRAY_BUFFER);

	m_fbo = new FrameBufferObject();
	m_fbo->SetActive();

	m_tex = new Texture(GL_TEXTURE_RECTANGLE);
	m_tex->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_fbo->AttachTexture(GL_COLOR_ATTACHMENT0, m_tex);

	m_vs = new Shader(GL_VERTEX_SHADER);
	m_vs->LoadFile(vs);
	m_fs = new Shader(GL_FRAGMENT_SHADER);
	m_fs->LoadFile(fs);

	m_pro = new ShaderProgram();
	m_pro->AddShader(m_vs);
	m_pro->AddShader(m_fs);
	m_pro->AddInAttribute("vertex_coord");
	m_pro->AddOutAttribute("fragment_colour");
	m_pro->Link();
}

PostProcessing::~PostProcessing() {
	delete m_vbo;
	delete m_vao;
	delete m_fbo;
	delete m_pro;
	delete m_tex;
	delete m_vs;
	delete m_fs;
}

#pragma endregion
#pragma region Getters/Setters

void PostProcessing::setSourceTexture(Texture *texture) {
	m_sTexture = texture;
}

void PostProcessing::setTextureSize(const GLsizei width, const GLsizei height) {
	m_width = width;
	m_height = height;
	m_tex->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
}

#pragma endregion
#pragma region Static Methods

#pragma endregion
#pragma region Non-Static Methods

void PostProcessing::Draw() {
	m_fbo->SetActive();
	m_pro->SetActive();
	m_vao->SetActive();

	m_pro->BindUniformTexture(m_sTexture, "frame");
	glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

	m_fbo->BlitTexture(m_tex, m_width, m_height);

	VertexArrayObject::Reset();
}

#pragma endregion