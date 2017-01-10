#include "MyView.hpp"

#include <scene/scene.hpp>
#include <tygra/FileHelper.hpp>
#include <tsl/shapes.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

#include "rendering\Shader.h"
#include "rendering\Texture.h"
#include "rendering\ShaderProgram.h"
#include "rendering\PostProcessing.h"
#include "rendering\FrameBufferObject.h"
#include "rendering\VertexArrayObject.h"
#include "rendering\VertexBufferObject.h"
#include "rendering\enums\Buffer.h"
#include "rendering\enums\Light.h"

#include "utilities\TimeQuery.h"

#pragma region Structs

struct MyView::Mesh {
	GLuint elementCount;
	GLuint instanceCount;
	GLuint elementIndex;
	GLuint vertexIndex;
	GLuint instanceIndex;
};

struct MyView::Shape {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject elements = VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject vertices = VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject instances = VertexBufferObject(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	GLuint elementCount;
};

struct MyView::Vertex {
	glm::vec3 positiion;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 textureCoordinate;
};

struct MyView::Indirect {
	std::vector<GLuint> ids;
	GLuint count;
	VertexBufferObject commands{ VertexBufferObject(GL_DRAW_INDIRECT_BUFFER, GL_STATIC_DRAW) };
};

struct MyView::Instance {
	glm::mat4 transform;
	GLint material;
};

struct MyView::StaticVOs {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject vbo[3] = {
		VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW),	///	Elements
		VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW),			///	Vertices
		VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW)				///	Instances
	};
};

struct MyView::NonStaticVOs {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject vbo[3] = {
		VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW),	///	Elements
		VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW),			///	Vertices
		VertexBufferObject(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW)			///	Instances
	};
};

struct MyView::DirectionalLight {
	glm::vec3 direction;
	glm::vec3 intensity;
};

struct MyView::PointLight {
	glm::vec3 position;
	glm::vec3 intensity;
	float range;
};

struct MyView::SpotLight {
	glm::vec3 position;
	glm::vec3 direction;
	glm::vec3 intensity;
	float range;
	float coneAngle;
};

#pragma endregion
#pragma region Constructors/Destructors

MyView::MyView() {
}

MyView::~MyView() {
}

#pragma endregion
#pragma region Getters/Setters

void MyView::setScene(const scene::Context * scene) {
    scene_ = scene;
}

void MyView::setMode(const Mode mode) {
	m_renderMode = mode;
}

#pragma endregion
#pragma region Window Methods

void MyView::windowViewWillStart(tygra::Window * window) {
    assert(scene_ != nullptr);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.f, 0.f, 0.25f, 0.f);

	PrepareVOs();
	PrepareTimers();

	DrawShadows(true);

	m_antiAliasing = std::make_unique<PostProcessing>("resource:///post_vs.glsl", "resource:///anti_aliasing_fs.glsl");
	m_antiAliasing->setSourceTexture(m_lBuffer.get());
	m_celShading = std::make_unique<PostProcessing>("resource:///post_vs.glsl", "resource:///cel_shading_fs.glsl");
	m_celShading->setSourceTexture(m_lBuffer.get());
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height) {
    glViewport(0, 0, width, height);
	screen_width = width;
	screen_height = height;

	const float aspect_ratio = (float)width / (float)height;
	projection_transform = glm::perspective(1.31f, aspect_ratio, 1.f, 1000.f);

	m_dbuffer->Buffer(GL_DEPTH24_STENCIL8, width, height, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);
	m_gBuffer[GBuffer::Position]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_gBuffer[GBuffer::Colour]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_gBuffer[GBuffer::Normal]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_gBuffer[GBuffer::Material]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_lBuffer->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_antiAliasing->setTextureSize(width, height);
}

void MyView::windowViewDidStop(tygra::Window * window) {

}

void MyView::windowViewRender(tygra::Window * window) {
    assert(scene_ != nullptr);

	UpdateViewTransform();

	switch (m_renderMode) {
	case Mode::Forward:
		ForwardRender();
		break;
	case Mode::Deferred:
		DeferredRender();
		if(m_postMode != PostProcess::Off) PostProcessRender();
		break;
	default:
		std::cerr << "No valid render mode selected." << std::endl;
	}
}

#pragma endregion
#pragma region Additional Methods

void MyView::LogTimers() {
	std::cout << "Forward Rendering: (" << m_queryForwardRender->toString() << ")" << std::endl;
	std::cout << "Deferred Rendering: (" << m_queryDeferredRender->toString() << ")" << std::endl;
	std::cout << "Post Processing: (" << m_queryPostProcessing->toString() << ")" << std::endl;
	std::cout << std::endl;
}

void MyView::ResetTimers() {
	m_queryForwardRender->Reset();
	m_queryDeferredRender->Reset();
	m_queryPostProcessing->Reset();
}

void MyView::TogglePostProcessing() {
	switch (m_postMode) {
		case PostProcess::Off:
			m_postMode = PostProcess::Anti_Aliasing;
			break;
		case PostProcess::Anti_Aliasing:
			m_postMode = PostProcess::Cel_Shading;
			break;
		case PostProcess::Cel_Shading:
			m_postMode = PostProcess::Off;
			break;
		default:
			m_postMode = PostProcess::Off;
			break;
	}
}

#pragma endregion
#pragma region Setup Methods

void MyView::PrepareVOs() {
	m_staticVOs = std::make_unique<StaticVOs>();
	m_nonStaticVOs = std::make_unique<NonStaticVOs>();

	m_lightVO[Light::Directional] = std::make_unique<Shape>();
	m_lightVO[Light::Point] = std::make_unique<Shape>();
	m_lightVO[Light::Spot] = std::make_unique<Shape>();

	PrepareGBs();
	PrepareMeshData();

	PrepareVBOs();
	PrepareVAOs();
	PrepareUBOs();

	PrepareShaders();
	PreparePrograms();

	PrepareTextures();
}

void MyView::PrepareVAOs() {
	m_staticVOs->vao.SetActive();
	m_staticVOs->vbo[Buffer::Element].SetActive();
	m_staticVOs->vbo[Buffer::Vertex].SetActive();
	m_staticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_staticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_staticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_staticVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 3));
	m_staticVOs->vbo[Buffer::Instance].SetActive();
	m_staticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE);
	m_staticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4)));
	m_staticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 2));
	m_staticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 3));
	m_staticVOs->vao.AddAttributeDivisor<Instance>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 4));

	m_nonStaticVOs->vao.SetActive();
	m_nonStaticVOs->vbo[Buffer::Element].SetActive();
	m_nonStaticVOs->vbo[Buffer::Vertex].SetActive();
	m_nonStaticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_nonStaticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_nonStaticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_nonStaticVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 3));
	m_nonStaticVOs->vbo[Buffer::Instance].SetActive();
	m_nonStaticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE);
	m_nonStaticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4)));
	m_nonStaticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 2));
	m_nonStaticVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 3));
	m_nonStaticVOs->vao.AddAttributeDivisor<Instance>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 4));

	m_lightVO[Light::Directional]->vao.SetActive();
	m_lightVO[Light::Directional]->vertices.SetActive();
	m_lightVO[Light::Directional]->vao.AddAttribute<glm::vec2>(2, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Directional]->instances.SetActive();
	m_lightVO[Light::Directional]->vao.AddAttributeDivisor<DirectionalLight>(3, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Directional]->vao.AddAttributeDivisor<DirectionalLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));

	m_lightVO[Light::Point]->vao.SetActive();
	m_lightVO[Light::Point]->elements.SetActive();
	m_lightVO[Light::Point]->vertices.SetActive();
	m_lightVO[Light::Point]->vao.AddAttribute<glm::vec3>(3, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Point]->instances.SetActive();
	m_lightVO[Light::Point]->vao.AddAttributeDivisor<PointLight>(3, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Point]->vao.AddAttributeDivisor<PointLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_lightVO[Light::Point]->vao.AddAttributeDivisor<PointLight>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));

	m_lightVO[Light::Spot]->vao.SetActive();
	m_lightVO[Light::Spot]->elements.SetActive();
	m_lightVO[Light::Spot]->vertices.SetActive();
	m_lightVO[Light::Spot]->vao.AddAttribute<glm::vec3>(3, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Spot]->instances.SetActive();
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<SpotLight>(3, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<SpotLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<SpotLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<SpotLight>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 3));
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<SpotLight>(1, GL_FLOAT, GL_FALSE, (int*)((sizeof(glm::vec3) * 3) + sizeof(float)));
	m_lightViewVbo->SetActive();
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE);
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4)));
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 2));
	m_lightVO[Light::Spot]->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 3));

	VertexArrayObject::Reset();
	VertexBufferObject::Reset(GL_ARRAY_BUFFER);
	VertexBufferObject::Reset(GL_ELEMENT_ARRAY_BUFFER);
}

void MyView::PrepareVBOs() {
	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;
	std::vector<Instance> instances;

	PrepareVertexData(*m_staticMeshes, vertices, elements, instances);
	m_staticVOs->vbo[Buffer::Vertex].BufferData(vertices);
	m_staticVOs->vbo[Buffer::Element].BufferData(elements);
	m_staticVOs->vbo[Buffer::Instance].BufferData(instances);
	vertices.clear();
	elements.clear();
	instances.clear();

	PrepareVertexData(*m_nonStaticMeshes, vertices, elements, instances);
	m_nonStaticVOs->vbo[Buffer::Vertex].BufferData(vertices);
	m_nonStaticVOs->vbo[Buffer::Element].BufferData(elements);
	m_nonStaticVOs->vbo[Buffer::Instance].BufferData(instances);
	vertices.clear();
	elements.clear();
	instances.clear();

	std::vector<glm::vec2> v = { glm::vec2(-1, -1),
		glm::vec2(1, -1),
		glm::vec2(1, 1),
		glm::vec2(-1, 1) };
	m_lightVO[Light::Directional]->vertices.BufferData(v);

	tsl::IndexedMeshPtr sphere = tsl::createSpherePtr(1.0f, 12);
	sphere = tsl::cloneIndexedMeshAsTriangleListPtr(sphere.get());

	m_lightVO[Light::Point]->vertices.BufferData(sphere->positionArray()[0], sphere->vertexCount());
	m_lightVO[Light::Point]->elements.BufferData(sphere->indexArray()[0], sphere->indexCount());
	m_lightVO[Light::Point]->elementCount = sphere->indexCount();

	tsl::IndexedMeshPtr cone = tsl::createConePtr(1.0f, 1.0f, 6);
	cone = tsl::cloneIndexedMeshAsTriangleListPtr(cone.get());

	std::vector<glm::vec3> positions;
	for (unsigned int i = 0; i < cone->vertexCount(); i++) {
		positions.push_back((const glm::vec3&)cone->positionArray()[i] - glm::vec3(0, 0, 1));
	}

	m_lightVO[Light::Spot]->vertices.BufferData(positions);
	m_lightVO[Light::Spot]->elements.BufferData(cone->indexArray()[0], cone->indexCount());
	m_lightVO[Light::Spot]->elementCount = cone->indexCount();

	m_lightViewVbo = std::make_unique<VertexBufferObject>(GL_VERTEX_ARRAY, GL_DYNAMIC_DRAW);
}

void MyView::PrepareUBOs() {
	m_materialUBO = std::make_unique<VertexBufferObject>(GL_UNIFORM_BUFFER, GL_STATIC_DRAW);
	m_materialUBO->BindRange(0, 0, sizeof(scene::Material) * 7);
	m_materialUBO->BufferData(scene_->getAllMaterials()[0], 7);
}

void MyView::PrepareTimers() {
	m_queryForwardRender = std::make_unique<TimeQuery>();
	m_queryDeferredRender = std::make_unique<TimeQuery>();
	m_queryPostProcessing = std::make_unique<TimeQuery>();
}

void MyView::PrepareShaders() {
	m_vsInstancedEnvironment = std::make_unique<Shader>(GL_VERTEX_SHADER);
	m_vsInstancedEnvironment->LoadFile("resource:///instanced_environment_vs.glsl");

	m_fsEnvironment = std::make_unique<Shader>(GL_FRAGMENT_SHADER);
	m_fsEnvironment->LoadFile("resource:///environment_fs.glsl");

	m_vsInstancedShadow = std::make_unique<Shader>(GL_VERTEX_SHADER);
	m_vsInstancedShadow->LoadFile("resource:///instanced_shadow_vs.glsl");

	m_fsShadow = std::make_unique<Shader>(GL_FRAGMENT_SHADER);
	m_fsShadow->LoadFile("resource:///shadow_fs.glsl");

	m_vsLight[Light::Directional] = std::make_unique<Shader>(GL_VERTEX_SHADER);
	m_vsLight[Light::Directional]->LoadFile("resource:///directional_light_vs.glsl");

	m_vsLight[Light::Point] = std::make_unique<Shader>(GL_VERTEX_SHADER);
	m_vsLight[Light::Point]->LoadFile("resource:///point_light_vs.glsl");

	m_vsLight[Light::Spot] = std::make_unique<Shader>(GL_VERTEX_SHADER);
	m_vsLight[Light::Spot]->LoadFile("resource:///spot_light_vs.glsl");

	m_fsLight[Light::Directional] = std::make_unique<Shader>(GL_FRAGMENT_SHADER);
	m_fsLight[Light::Directional]->LoadFile("resource:///directional_light_fs.glsl");

	m_fsLight[Light::Point] = std::make_unique<Shader>(GL_FRAGMENT_SHADER);
	m_fsLight[Light::Point]->LoadFile("resource:///point_light_fs.glsl");

	m_fsLight[Light::Spot] = std::make_unique<Shader>(GL_FRAGMENT_SHADER);
	m_fsLight[Light::Spot]->LoadFile("resource:///spot_light_fs.glsl");
}

void MyView::PreparePrograms() {
	m_environmentProgram = std::make_unique<ShaderProgram>();
	ShaderProgram *p = m_environmentProgram.get();

	p->AddShader(m_vsInstancedEnvironment.get(), m_fsEnvironment.get());

	p->AddInAttribute("vertex_position", "vertex_normal", "vertex_tangent", "vertex_texture_coordinate", "model");
	p->AddOutAttribute("fragment_colour", "fragment_position", "fragment_normal", "fragment_material");

	p->Link();

	p->BindBlock(m_materialUBO.get(), "block_material");

	m_shadowProgram = std::make_unique<ShaderProgram>();
	p = m_shadowProgram.get();

	p->AddShader(m_vsInstancedShadow.get(), m_fsShadow.get());

	p->AddInAttribute("vertex_position", "model");
	p->AddOutAttribute("fragment_depth");

	p->Link();

	m_lightProgram[Light::Directional] = std::make_unique<ShaderProgram>();
	p = m_lightProgram[Light::Directional].get();

	p->AddShader(m_vsLight[Light::Directional].get(), m_fsLight[Light::Directional].get());

	p->AddInAttribute("vertex_coord", "light.direction", "light.intensity");
	p->AddOutAttribute("fragment_colour");

	p->Link();

	p->BindBlock(m_materialUBO.get(), "block_material");

	m_lightProgram[Light::Point] = std::make_unique<ShaderProgram>();
	p = m_lightProgram[Light::Point].get();

	p->AddShader(m_vsLight[Light::Point].get(), m_fsLight[Light::Point].get());

	p->AddInAttribute("vertex_coord", "light.position", "light.intensity", "light.range");
	p->AddOutAttribute("fragment_colour");

	p->Link();

	p->BindBlock(m_materialUBO.get(), "block_material");

	m_lightProgram[Light::Spot] = std::make_unique<ShaderProgram>();
	p = m_lightProgram[Light::Spot].get();

	p->AddShader(m_vsLight[Light::Spot].get(), m_fsLight[Light::Spot].get());

	p->AddInAttribute("vertex_coord", "light.position", "light.direction", "light.intensity", "light.range", "light.coneAngle", "source_projection");
	p->AddOutAttribute("fragment_colour");

	p->Link();

	p->BindBlock(m_materialUBO.get(), "block_material");
}

void MyView::PrepareMeshData() {
	scene::GeometryBuilder builder;
	const auto &meshes = builder.getAllMeshes();

	m_staticMeshes = std::make_unique<Indirect>();
	m_nonStaticMeshes = std::make_unique<Indirect>();

	for (const auto &mesh : meshes) {
		GLuint id = mesh.getId();
		GLboolean isStatic = true;

		for (const GLuint index : scene_->getInstancesByMeshId(id)) {
			if (!scene_->getInstanceById(index).isStatic()) {
				isStatic = false;
			}
		}

		isStatic ?
			m_staticMeshes->ids.push_back(id):
			m_nonStaticMeshes->ids.push_back(id);
	}
}

void MyView::PrepareTextures() {
	std::string mainTexture[7] = { "content:///brick.png", "content:///wall.png", "content:///not_fabric.png" };
	std::string normalTexture[7] = { "content:///brick_norm.png", "content:///wall_norm.png", "content:///not_fabric_norm.png" };
	
	for (unsigned int i = 0; i < 7; i++) {
		if (mainTexture[i].size() > 0) {
			m_mainTexture[i] = std::make_unique<Texture>(GL_TEXTURE_2D);
			m_mainTexture[i]->LoadFile(mainTexture[i]);
		}

		if (normalTexture[i].size() > 0) {
			m_normalTexture[i] = std::make_unique<Texture>(GL_TEXTURE_2D);
			m_normalTexture[i]->LoadFile(normalTexture[i]);
		}
	}

	m_environmentProgram->SetActive();
	for (unsigned int i = 0, j = 0; i < 7; i++, j++) {
		if (m_mainTexture[i] == nullptr || m_normalTexture[i] == nullptr)
			break;

		std::string main = "mainTexture[" + std::to_string(i) + "]";
		std::string normal = "normalTexture[" + std::to_string(i) + "]";

		m_environmentProgram->BindUniformTexture(m_mainTexture[i].get(), main, (i * 2));
		m_environmentProgram->BindUniformTexture(m_normalTexture[i].get(), normal, (i * 2) + 1);
	}

	m_lightProgram[Light::Spot]->SetActive();
	for (int i = 0; i < 5; i++) {
		m_shadowTexture[i] = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_DEPTH_ATTACHMENT);
		m_shadowTexture[i]->Buffer(GL_DEPTH_COMPONENT16, 1024, 1024, GL_DEPTH_COMPONENT, GL_FLOAT);

		m_sFbo->AttachTexture(m_shadowTexture[i].get(), false);
		m_sFbo->SetDraw();

		glClear(GL_DEPTH_BUFFER_BIT);

		std::string texture = "shadowMap[" + std::to_string(i) + "]";
		std::string transform = "shadowTransform[" + std::to_string(i) + "]";

		m_lightProgram[Light::Spot]->BindUniformM4(glm::mat4(0), transform);
		m_lightProgram[Light::Spot]->BindUniformTexture(m_shadowTexture[i].get(), texture, i + 4);
	}
}

void MyView::PrepareGBs() {
	m_dbuffer = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_DEPTH_STENCIL_ATTACHMENT);
	m_dbuffer->Buffer(GL_DEPTH24_STENCIL8, 0, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8);

	m_gFbo = std::make_unique<FrameBufferObject>();

	m_gFbo->AttachTexture(m_dbuffer.get(), false);

	m_gBuffer[GBuffer::Colour] = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT0);
	m_gBuffer[GBuffer::Colour]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Colour].get());

	m_gBuffer[GBuffer::Position] = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT1);
	m_gBuffer[GBuffer::Position]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Position].get());

	m_gBuffer[GBuffer::Normal] = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT2);
	m_gBuffer[GBuffer::Normal]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Normal].get());

	m_gBuffer[GBuffer::Material] = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT3);
	m_gBuffer[GBuffer::Material]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Material].get());

	m_lFbo = std::make_unique<FrameBufferObject>();

	m_lFbo->AttachTexture(m_dbuffer.get(), false);

	m_lBuffer = std::make_unique<Texture>(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT0);
	m_lBuffer->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_lFbo->AttachTexture(m_lBuffer.get());

	m_sFbo = std::make_unique<FrameBufferObject>();
}

void MyView::PrepareVertexData(Indirect &indirect, std::vector<Vertex> &vertices, std::vector<GLuint> &elements, std::vector<Instance> &instances) {
	scene::GeometryBuilder builder;
	std::vector<Mesh> commands;
	for (GLuint &id : indirect.ids) {
		const auto &mesh = builder.getMeshById(id);
		Mesh m;

		m.vertexIndex = (GLuint)vertices.size();
		m.elementIndex = (GLuint)elements.size();
		m.instanceIndex = (GLuint)instances.size();

		const auto &meshElements = mesh.getElementArray();
		for (const auto& element : meshElements)
			elements.push_back(element);
		m.elementCount = (GLuint)meshElements.size();

		const auto &positions = mesh.getPositionArray();
		const auto &normals = mesh.getNormalArray();
		const auto &tangents = mesh.getTangentArray();
		const auto &textureCoordinates = mesh.getTextureCoordinateArray();

		const unsigned int vertexCount = (GLuint)positions.size();

		if (textureCoordinates.size() == 0) {
			for (unsigned int i = 0; i < vertexCount; i++) {
				Vertex v;
				v.positiion = (const glm::vec3&)positions[i];
				v.normal = (const glm::vec3&)normals[i];
				v.tangent = glm::vec3(0);
				v.textureCoordinate = glm::vec2(0);
				vertices.push_back(v);
			}
		} else {
			for (unsigned int i = 0; i < vertexCount; i++) {
				Vertex v;
				v.positiion = (const glm::vec3&)positions[i];
				v.normal = (const glm::vec3&)normals[i];
				v.tangent = (const glm::vec3&)tangents[i];
				v.textureCoordinate = (const glm::vec2&)textureCoordinates[i];
				vertices.push_back(v);
			}
		}

		const auto &meshInstances = scene_->getInstancesByMeshId(id);
		for (const auto &instanceID : meshInstances) {
			const auto &instance = scene_->getInstanceById(instanceID);
			Instance i;
			i.transform = glm::mat4((const glm::mat4x3&)instance.getTransformationMatrix());
			i.material = instance.getMaterialId();
			instances.push_back(i);
		}
		m.instanceCount = (GLuint)meshInstances.size();
		commands.push_back(m);
	}
	indirect.commands.BufferData(commands);
	indirect.count = commands.size();
}

#pragma endregion
#pragma region Render Methods

void MyView::ForwardRender() {
	m_queryForwardRender->Begin();

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	DrawEnvironment();

	m_queryForwardRender->End();
}

void MyView::DeferredRender() {
	m_queryDeferredRender->Begin();

	m_gFbo->SetActive();

	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);

	glStencilFunc(GL_ALWAYS, 1, ~0);

	DrawEnvironment();

	m_gFbo->BlitTexture(m_gBuffer[GBuffer::Colour].get(), screen_width, screen_height, m_lFbo->getID());

	UpdateLights();

	DrawShadows();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	glStencilFunc(GL_LEQUAL, 1, ~0);

	m_gFbo->SetRead();
	m_lFbo->SetDraw();

	DrawLights();

	glDisable(GL_BLEND);
	glDisable(GL_STENCIL_TEST);

	m_lFbo->BlitTexture(m_lBuffer.get(), screen_width, screen_height);

	FrameBufferObject::Reset();

	m_queryDeferredRender->End();
}

void MyView::PostProcessRender() {
	m_queryPostProcessing->Begin();
	
	switch (m_postMode) {
		case PostProcess::Anti_Aliasing:
			m_antiAliasing->Draw();
			break;
		case PostProcess::Cel_Shading:
			m_celShading->Draw();
			break;
	}

	m_lFbo->BlitTexture(m_lBuffer.get(), screen_width, screen_height);

	m_queryPostProcessing->End();
}

void MyView::DrawEnvironment() {
	glm::mat4 combined_transform = projection_transform * view_transform;

	m_environmentProgram->SetActive();
	m_environmentProgram->BindUniformM4(combined_transform, "combined_transform");
	m_environmentProgram->BindUniformV3(scene_->getAmbientLightIntensity(), "ambience");

	m_staticVOs->vao.SetActive();
	m_staticMeshes->commands.SetActive();
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, m_staticMeshes->count, sizeof(Mesh));

	UpdateNonStaticTransforms();
	m_nonStaticVOs->vao.SetActive();
	m_nonStaticMeshes->commands.SetActive();
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, m_nonStaticMeshes->count, sizeof(Mesh));

	VertexArrayObject::Reset();
}

void MyView::DrawShadows(bool drawStatic) {
	glViewport(0, 0, 1024, 1024);

	for (int i = 0; i < 5; i++) {
		auto &light = scene_->getAllSpotLights()[i];

		bool cast = light.getCastShadow();
		bool isStatic = light.isStatic();

		if (!cast || (isStatic && !drawStatic)) {
			continue;
		}

		glm::vec3 pos = (const glm::vec3&)light.getPosition();
		glm::vec3 dir = (const glm::vec3&)light.getDirection();
		float ran = light.getRange();
		float fov = glm::radians(light.getConeAngleDegrees());

		glm::mat4 depthProjectionMatrix = glm::perspective<float>(fov, 1.0f, 1.0f, ran);
		glm::mat4 depthViewMatrix = glm::lookAt(pos, pos + dir, glm::vec3(0, 1, 0));

		glm::mat4 combined_transform = depthProjectionMatrix * depthViewMatrix;

		m_sFbo->AttachTexture(m_shadowTexture[i].get(), false);
		m_sFbo->SetDraw();

		glClear(GL_DEPTH_BUFFER_BIT);

		m_shadowProgram->SetActive();
		m_shadowProgram->BindUniformM4(combined_transform, "combined_transform");

		m_staticVOs->vao.SetActive();
		m_staticMeshes->commands.SetActive();
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, m_staticMeshes->count, sizeof(Mesh));

		UpdateNonStaticTransforms();
		m_nonStaticVOs->vao.SetActive();
		m_nonStaticMeshes->commands.SetActive();
		glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, m_nonStaticMeshes->count, sizeof(Mesh));
	}

	m_lightProgram[Light::Spot]->SetActive();
	for (int i = 0; i < 5; i++) {
		std::string texture = "shadowMap[" + std::to_string(i) + "]";
		m_lightProgram[Light::Spot]->BindUniformTexture(m_shadowTexture[i].get(), texture, i + 4);
	}

	glViewport(0, 0, screen_width, screen_height);
	VertexArrayObject::Reset();
}

void MyView::DrawLights() {
	glm::mat4 combined_transform = projection_transform * view_transform;

	m_lightProgram[Light::Directional]->SetActive();
	m_lightVO[Light::Directional]->vao.SetActive();

	m_lightProgram[Light::Directional]->BindUniformV3(scene_->getCamera().getPosition(), "eyePosition");
	m_lightProgram[Light::Directional]->BindUniformV3(scene_->getCamera().getDirection(), "eyeDirection");
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Colour].get(), "colourMap");
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Position].get(), "positionMap", 1);
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Normal].get(), "normalMap", 2);
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Material].get(), "materialMap", 3);

	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 2);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_GEQUAL);

	m_lightProgram[Light::Point]->SetActive();
	m_lightVO[Light::Point]->vao.SetActive();

	m_lightProgram[Light::Point]->BindUniformM4(combined_transform, "combined_transform");
	m_lightProgram[Light::Point]->BindUniformV3(scene_->getCamera().getPosition(), "eyePosition");
	m_lightProgram[Light::Point]->BindUniformV3(scene_->getCamera().getDirection(), "eyeDirection");
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Colour].get(), "colourMap");
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Position].get(), "positionMap", 1);
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Normal].get(), "normalMap", 2);
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Material].get(), "materialMap", 3);

	glDrawElementsInstanced(GL_TRIANGLES, m_lightVO[Light::Point]->elementCount, GL_UNSIGNED_INT, 0, 20);

	m_lightProgram[Light::Spot]->SetActive();
	m_lightVO[Light::Spot]->vao.SetActive();

	m_lightProgram[Light::Spot]->BindUniformM4(combined_transform, "combined_transform");
	m_lightProgram[Light::Spot]->BindUniformV3(scene_->getCamera().getPosition(), "eyePosition");
	m_lightProgram[Light::Spot]->BindUniformV3(scene_->getCamera().getDirection(), "eyeDirection");
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Colour].get(), "colourMap");
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Position].get(), "positionMap", 1);
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Normal].get(), "normalMap", 2);
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Material].get(), "materialMap", 3);

	glDrawElementsInstanced(GL_TRIANGLES, m_lightVO[Light::Spot]->elementCount, GL_UNSIGNED_INT, 0, 5);

	glCullFace(GL_BACK);
	glDepthFunc(GL_LEQUAL);

	VertexArrayObject::Reset();
}

#pragma endregion
#pragma region Additional Methods

void MyView::UpdateViewTransform() {
	scene::Camera camera = scene_->getCamera();
	glm::vec3 camera_pos = (const glm::vec3&)camera.getPosition();
	glm::vec3 camera_dir = (const glm::vec3&)camera.getDirection();
	view_transform = glm::lookAt(camera_pos, camera_pos + camera_dir, glm::vec3(0, 1, 0));
}

void MyView::UpdateNonStaticTransforms() {
	std::vector<Instance> instances;
	for (const GLuint &meshID : m_nonStaticMeshes->ids) {
		for (const GLuint &id : scene_->getInstancesByMeshId(meshID)) {
			const auto &instance = scene_->getInstanceById(id);
			Instance i;
			i.transform = (const glm::mat4x3&)instance.getTransformationMatrix();
			i.material = instance.getMaterialId();
			instances.push_back(i);
		}
	}
	m_nonStaticVOs->vbo[Buffer::Instance].BufferData(instances);
}

void MyView::UpdateLights() {
	std::vector<DirectionalLight> dLights;
	for (const auto &light : scene_->getAllDirectionalLights()) {
		DirectionalLight d;
		d.direction = (const glm::vec3&)light.getDirection();
		d.intensity = (const glm::vec3&)light.getIntensity();
		dLights.push_back(d);
	}
	m_lightVO[Light::Directional]->instances.BufferData(dLights);

	std::vector<PointLight> pLights;
	for (const auto &light : scene_->getAllPointLights()) {
		PointLight p;
		p.position = (const glm::vec3&)light.getPosition();
		p.intensity = (const glm::vec3&)light.getIntensity();
		p.range = light.getRange();
		pLights.push_back(p);
	}
	m_lightVO[Light::Point]->instances.BufferData(pLights);

	std::vector<SpotLight> sLights;
	std::vector<glm::mat4> transforms;
	glm::mat4 bias(0.5f);
	bias[3] += glm::vec4(0.5f);
	for (const auto &light : scene_->getAllSpotLights()) {
		SpotLight s;
		s.position = (const glm::vec3&)light.getPosition();
		s.direction = (const glm::vec3&)light.getDirection();
		s.intensity = (const glm::vec3&)light.getIntensity();
		s.range = light.getRange();
		s.coneAngle = glm::radians(light.getConeAngleDegrees());
		sLights.push_back(s);


		glm::mat4 depthProjectionMatrix = glm::perspective<float>(s.coneAngle, 1.0f, 1.0f, s.range);
		glm::mat4 depthViewMatrix = glm::lookAt(s.position, s.position + s.direction, glm::vec3(0, 1, 0));

		transforms.push_back(bias * depthProjectionMatrix * depthViewMatrix);
	}
	m_lightVO[Light::Spot]->instances.BufferData(sLights);
	m_lightViewVbo->BufferData(transforms);
}

#pragma endregion