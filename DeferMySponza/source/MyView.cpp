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

#include "utilities\TimeQuery.h"

#pragma region Structs

struct MyView::Mesh {
	GLuint id;

	GLuint elementCount;
	GLuint elementIndex;

	GLuint vertexIndex;

	GLuint instanceCount;
	GLuint instanceIndex;
};

struct MyView::Vertex {
	glm::vec3 positiion;
	glm::vec3 normal;
	glm::vec3 tangent;
	glm::vec2 textureCoordinate;
};

struct MyView::Instance {
	glm::mat4 transform;
	GLint material;
};

struct MyView::InstanceVOs {
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

struct MyView::NonInstanceVOs {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject vbo[2] = {
		VertexBufferObject(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW),	///	Elements
		VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW)				///	Vertices
	};
	std::vector<Instance> instances;
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

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.f, 0.f, 0.25f, 0.f);

	PrepareVOs();
	PrepareTimers();

	m_antiAliasing = new PostProcessing("resource:///post_vs.glsl", "resource:///anti_aliasing_fs.glsl");
	m_antiAliasing->setSourceTexture(m_lBuffer);
	m_celShading = new PostProcessing("resource:///post_vs.glsl", "resource:///cel_shading_fs.glsl");
	m_celShading->setSourceTexture(m_lBuffer);
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height) {
    glViewport(0, 0, width, height);
	view_size = glm::vec2(width, height);

	const float aspect_ratio = (float)width / (float)height;
	projection_transform = glm::perspective(1.31f, aspect_ratio, 1.f, 1000.f);

	m_dbuffer->Buffer(GL_DEPTH_COMPONENT32F, width, height, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_gBuffer[GBuffer::Position]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_gBuffer[GBuffer::Colour]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_gBuffer[GBuffer::Normal]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_lBuffer->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
	m_antiAliasing->setTextureSize(width, height);
}

void MyView::windowViewDidStop(tygra::Window * window) {
	delete m_antiAliasing;
	delete m_celShading;

	delete m_gFbo;
	delete m_lFbo;
	delete m_dbuffer;
	delete m_lBuffer;
	delete m_gBuffer[GBuffer::Colour];
	delete m_gBuffer[GBuffer::Position];
	delete m_gBuffer[GBuffer::Normal];

	delete m_instancedVOs;
	delete m_nonStaticVOs;
	delete m_nonInstancedVOs;

	delete m_lightVAO[Light::Directional];
	delete m_lightVAO[Light::Point];
	delete m_lightVAO[Light::Spot];
	delete m_lightVBO[Light::Directional];
	delete m_lightVBO[Light::Point];
	delete m_lightVBO[Light::Spot];
	delete m_quadVBO;

	delete m_materialUBO;
	for (Texture *ptr : m_mainTexture) {
		delete ptr;
	}

	delete m_lightProgram[Light::Directional];
	delete m_lightProgram[Light::Point];
	delete m_lightProgram[Light::Spot];

	delete m_vsLight;
	delete m_fsLight[Light::Directional];
	delete m_fsLight[Light::Point];
	delete m_fsLight[Light::Spot];

	delete m_environmentProgram[Program::Instanced];
	delete m_environmentProgram[Program::NonInstanced];

	delete m_vsInstanced;
	delete m_vsNonInstanced;
	delete m_fsEnvironment;

	delete m_queryForwardRender;
	delete m_queryDeferredRender;
	delete m_queryPostProcessing;
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
	m_instancedVOs = new InstanceVOs();
	m_nonStaticVOs = new NonStaticVOs();
	m_nonInstancedVOs = new NonInstanceVOs();

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
	m_instancedVOs->vao.SetActive();
	m_instancedVOs->vbo[Buffer::Element].SetActive();
	m_instancedVOs->vbo[Buffer::Vertex].SetActive();
	m_instancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_instancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_instancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_instancedVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 3));
	m_instancedVOs->vbo[Buffer::Instance].SetActive();
	m_instancedVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE);
	m_instancedVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4)));
	m_instancedVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 2));
	m_instancedVOs->vao.AddAttributeDivisor<Instance>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 3));
	m_instancedVOs->vao.AddAttributeDivisor<Instance>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 4));

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

	m_nonInstancedVOs->vao.SetActive();
	m_nonInstancedVOs->vbo[Buffer::Element].SetActive();
	m_nonInstancedVOs->vbo[Buffer::Vertex].SetActive();
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 3));

	m_lightVAO[Light::Directional] = new VertexArrayObject();
	m_lightVAO[Light::Directional]->SetActive();
	m_quadVBO->SetActive();
	m_lightVAO[Light::Directional]->AddAttribute<glm::vec2>(2, GL_FLOAT, GL_FALSE);
	m_lightVBO[Light::Directional]->SetActive();
	m_lightVAO[Light::Directional]->AddAttributeDivisor<DirectionalLight>(3, GL_FLOAT, GL_FALSE);
	m_lightVAO[Light::Directional]->AddAttributeDivisor<DirectionalLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));

	m_lightVAO[Light::Point] = new VertexArrayObject();
	m_lightVAO[Light::Point]->SetActive();
	m_quadVBO->SetActive();
	m_lightVAO[Light::Point]->AddAttribute<glm::vec2>(2, GL_FLOAT, GL_FALSE);
	m_lightVBO[Light::Point]->SetActive();
	m_lightVAO[Light::Point]->AddAttributeDivisor<PointLight>(3, GL_FLOAT, GL_FALSE);
	m_lightVAO[Light::Point]->AddAttributeDivisor<PointLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_lightVAO[Light::Point]->AddAttributeDivisor<PointLight>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));

	m_lightVAO[Light::Spot] = new VertexArrayObject();
	m_lightVAO[Light::Spot]->SetActive();
	m_quadVBO->SetActive();
	m_lightVAO[Light::Spot]->AddAttribute<glm::vec2>(2, GL_FLOAT, GL_FALSE);
	m_lightVBO[Light::Spot]->SetActive();
	m_lightVAO[Light::Spot]->AddAttributeDivisor<SpotLight>(3, GL_FLOAT, GL_FALSE);
	m_lightVAO[Light::Spot]->AddAttributeDivisor<SpotLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_lightVAO[Light::Spot]->AddAttributeDivisor<SpotLight>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_lightVAO[Light::Spot]->AddAttributeDivisor<SpotLight>(1, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 3));
	m_lightVAO[Light::Spot]->AddAttributeDivisor<SpotLight>(1, GL_FLOAT, GL_FALSE, (int*)((sizeof(glm::vec3) * 3) + sizeof(float)));

	VertexArrayObject::Reset();
	VertexBufferObject::Reset(GL_ARRAY_BUFFER);
	VertexBufferObject::Reset(GL_ELEMENT_ARRAY_BUFFER);
}

void MyView::PrepareVBOs() {
	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;
	std::vector<Instance> instances;

	PrepareVertexData(m_instancedMeshes, vertices, elements, instances);
	m_instancedVOs->vbo[Buffer::Vertex].BufferData(vertices);
	m_instancedVOs->vbo[Buffer::Element].BufferData(elements);
	m_instancedVOs->vbo[Buffer::Instance].BufferData(instances);
	vertices.clear();
	elements.clear();
	instances.clear();

	PrepareVertexData(m_nonStaticMeshes, vertices, elements, instances);
	m_nonStaticVOs->vbo[Buffer::Vertex].BufferData(vertices);
	m_nonStaticVOs->vbo[Buffer::Element].BufferData(elements);
	m_nonStaticVOs->vbo[Buffer::Instance].BufferData(instances);
	vertices.clear();
	elements.clear();
	instances.clear();

	PrepareVertexData(m_nonInstancedMeshes, vertices, elements, instances);
	m_nonInstancedVOs->vbo[Buffer::Vertex].BufferData(vertices);
	m_nonInstancedVOs->vbo[Buffer::Element].BufferData(elements);
	m_nonInstancedVOs->instances = instances;
	vertices.clear();
	elements.clear();
	instances.clear();

	m_quadVBO = new VertexBufferObject(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	std::vector<glm::vec2> v = { glm::vec2(-1, -1),
		glm::vec2(1, -1),
		glm::vec2(1, 1),
		glm::vec2(-1, 1) };
	m_quadVBO->BufferData(v);
	m_lightVBO[Light::Directional] = new VertexBufferObject(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	m_lightVBO[Light::Point] = new VertexBufferObject(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
	m_lightVBO[Light::Spot] = new VertexBufferObject(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
}

void MyView::PrepareUBOs() {
	m_materialUBO = new VertexBufferObject(GL_UNIFORM_BUFFER, GL_STATIC_DRAW);
	m_materialUBO->BindRange(0, 0, sizeof(scene::Material) * 7);
	m_materialUBO->BufferData(scene_->getAllMaterials()[0], 7);
}

void MyView::PrepareTimers() {
	m_queryForwardRender = new TimeQuery();
	m_queryDeferredRender = new TimeQuery();
	m_queryPostProcessing = new TimeQuery();
}

void MyView::PrepareShaders() {
	m_vsInstanced = new Shader(GL_VERTEX_SHADER);
	m_vsInstanced->LoadFile("resource:///instanced_environment_vs.glsl");

	m_vsNonInstanced = new Shader(GL_VERTEX_SHADER);
	m_vsNonInstanced->LoadFile("resource:///non_instanced_environment_vs.glsl");

	m_fsEnvironment = new Shader(GL_FRAGMENT_SHADER);
	m_fsEnvironment->LoadFile("resource:///environment_fs.glsl");

	m_vsLight = new Shader(GL_VERTEX_SHADER);
	m_vsLight->LoadFile("resource:///light_vs.glsl");

	m_fsLight[Light::Directional] = new Shader(GL_FRAGMENT_SHADER);
	m_fsLight[Light::Directional]->LoadFile("resource:///directional_light_fs.glsl");

	m_fsLight[Light::Point] = new Shader(GL_FRAGMENT_SHADER);
	m_fsLight[Light::Point]->LoadFile("resource:///point_light_fs.glsl");

	m_fsLight[Light::Spot] = new Shader(GL_FRAGMENT_SHADER);
	m_fsLight[Light::Spot]->LoadFile("resource:///spot_light_fs.glsl");
}

void MyView::PreparePrograms() {
	ShaderProgram *p = new ShaderProgram();
	m_environmentProgram[Program::Instanced] = p;

	p->AddShader(m_vsInstanced);
	p->AddShader(m_fsEnvironment);

	p->AddInAttribute("vertex_position");
	p->AddInAttribute("vertex_normal");
	p->AddInAttribute("vertex_tangent");
	p->AddInAttribute("vertex_texture_coordinate");

	p->AddInAttribute("model");

	p->AddOutAttribute("fragment_colour");
	p->AddOutAttribute("fragment_position");
	p->AddOutAttribute("fragment_normal");

	p->Link();

	p->BindBlock(m_materialUBO, "block_material");

	p = new ShaderProgram();
	m_environmentProgram[Program::NonInstanced] = p;

	p->AddShader(m_vsNonInstanced);
	p->AddShader(m_fsEnvironment);

	p->AddInAttribute("vertex_position");
	p->AddInAttribute("vertex_normal");
	p->AddInAttribute("vertex_tangent");
	p->AddInAttribute("vertex_texture_coordinate");

	p->AddOutAttribute("fragment_colour");
	p->AddOutAttribute("fragment_position");
	p->AddOutAttribute("fragment_normal");
	p->AddOutAttribute("fragment_coordinate");

	p->Link();

	p->BindBlock(m_materialUBO, "block_material");

	p = new ShaderProgram();
	m_lightProgram[Light::Directional] = p;

	p->AddShader(m_vsLight);
	p->AddShader(m_fsLight[Light::Directional]);

	p->AddInAttribute("vertex_coord");
	p->AddInAttribute("light.direction");
	p->AddInAttribute("light.intensity");

	p->AddOutAttribute("fragment_colour");

	p->Link();

	p = new ShaderProgram();
	m_lightProgram[Light::Point] = p;

	p->AddShader(m_vsLight);
	p->AddShader(m_fsLight[Light::Point]);

	p->AddInAttribute("vertex_coord");
	p->AddInAttribute("light.position");
	p->AddInAttribute("light.intensity");
	p->AddInAttribute("light.range");

	p->AddOutAttribute("fragment_colour");

	p->Link();

	p = new ShaderProgram();
	m_lightProgram[Light::Spot] = p;

	p->AddShader(m_vsLight);
	p->AddShader(m_fsLight[Light::Spot]);

	p->AddInAttribute("vertex_coord");
	p->AddInAttribute("light.position");
	p->AddInAttribute("light.direction");
	p->AddInAttribute("light.intensity");
	p->AddInAttribute("light.range");
	p->AddInAttribute("light.coneAngle");

	p->AddOutAttribute("fragment_colour");

	p->Link();
}

void MyView::PrepareMeshData() {
	scene::GeometryBuilder builder;
	const auto &meshes = builder.getAllMeshes();

	for (const auto &mesh : meshes) {
		Mesh m;
		m.id = mesh.getId();
		GLboolean isStatic = true;
		GLboolean instance = scene_->getInstancesByMeshId(m.id).size() > 1;

		for (const GLuint index : scene_->getInstancesByMeshId(m.id)) {
			if (!scene_->getInstanceById(index).isStatic()) {
				isStatic = false;
			}
		}

		isStatic ?
			instance ?
				m_instancedMeshes.push_back(m):
				m_nonInstancedMeshes.push_back(m):
			m_nonStaticMeshes.push_back(m);
	}
}

void MyView::PrepareTextures() {
	std::string mainTexture[7] = { "content:///brick.png", "content:///wall.png", "content:///not_fabric.png" };
	std::string normalTexture[7] = { "content:///brick_norm.png", "content:///wall_norm.png", "content:///not_fabric_norm.png" };
	
	for (unsigned int i = 0; i < 7; i++) {
		if (mainTexture[i].size() > 0) {
			m_mainTexture[i] = new Texture(GL_TEXTURE_2D);
			m_mainTexture[i]->LoadFile(mainTexture[i]);
		}

		if (normalTexture[i].size() > 0) {
			m_normalTexture[i] = new Texture(GL_TEXTURE_2D);
			m_normalTexture[i]->LoadFile(normalTexture[i]);
		}
	}

	ShaderProgram *ptrs[2] = { m_environmentProgram[Program::Instanced], m_environmentProgram[Program::NonInstanced] };
	for (ShaderProgram *ptr : ptrs) {
		ptr->SetActive();
		for (unsigned int i = 0, j = 0; i < 7; i++, j++) {
			if (m_mainTexture[i] == nullptr || m_normalTexture[i] == nullptr)
				break;

			std::string main = "mainTexture[" + std::to_string(i) + "]";
			std::string normal = "normalTexture[" + std::to_string(i) + "]";

			ptr->BindUniformTexture(m_mainTexture[i], main, (i * 2));
			ptr->BindUniformTexture(m_normalTexture[i], normal, (i * 2) + 1);
		}
	}
}

void MyView::PrepareGBs() {
	m_gFbo = new FrameBufferObject();

	m_dbuffer = new Texture(GL_TEXTURE_RECTANGLE, GL_DEPTH_ATTACHMENT);
	m_dbuffer->Buffer(GL_DEPTH_COMPONENT32F, 0, 0, GL_DEPTH_COMPONENT, GL_FLOAT);
	m_gFbo->AttachTexture(m_dbuffer, false);

	m_gBuffer[GBuffer::Colour] = new Texture(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT0);
	m_gBuffer[GBuffer::Colour]->SetActive();
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer[GBuffer::Colour]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Colour]);

	m_gBuffer[GBuffer::Position] = new Texture(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT1);
	m_gBuffer[GBuffer::Position]->SetActive();
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer[GBuffer::Position]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Position]);

	m_gBuffer[GBuffer::Normal] = new Texture(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT2);
	m_gBuffer[GBuffer::Normal]->SetActive();
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_gBuffer[GBuffer::Normal]->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_gFbo->AttachTexture(m_gBuffer[GBuffer::Normal]);

	m_lFbo = new FrameBufferObject();

	m_lBuffer = new Texture(GL_TEXTURE_RECTANGLE, GL_COLOR_ATTACHMENT0);
	m_lBuffer->SetActive();
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	m_lBuffer->Buffer(GL_RGB32F, 0, 0, GL_RGB, GL_FLOAT);
	m_lFbo->AttachTexture(m_lBuffer);
}

void MyView::PrepareVertexData(std::vector<Mesh> &meshData, std::vector<Vertex> &vertices, std::vector<GLuint> &elements, std::vector<Instance> &instances) {
	scene::GeometryBuilder builder;
	for (Mesh &m : meshData) {
		const auto &mesh = builder.getMeshById(m.id);

		m.vertexIndex = vertices.size();
		m.elementIndex = elements.size();
		m.instanceIndex = instances.size();

		const auto &meshElements = mesh.getElementArray();
		for (const auto& element : meshElements)
			elements.push_back(element);
		m.elementCount = meshElements.size();

		const auto &positions = mesh.getPositionArray();
		const auto &normals = mesh.getNormalArray();
		const auto &tangents = mesh.getTangentArray();
		const auto &textureCoordinates = mesh.getTextureCoordinateArray();

		const unsigned int vertexCount = positions.size();

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

		const auto &meshInstances = scene_->getInstancesByMeshId(m.id);
		for (const auto &instanceID : meshInstances) {
			const auto &instance = scene_->getInstanceById(instanceID);
			Instance i;
			i.transform = glm::mat4((const glm::mat4x3&)instance.getTransformationMatrix());
			i.material = instance.getMaterialId();
			instances.push_back(i);
		}
		m.instanceCount = meshInstances.size();
	}
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	DrawEnvironment();

	m_gFbo->BlitTexture(m_gBuffer[GBuffer::Colour], view_size.x, view_size.y, m_lFbo->getID());

	m_gFbo->SetRead();
	m_lFbo->SetDraw();

	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	UpdateLights();

	DrawLights();

	glDisable(GL_BLEND);

	m_lFbo->BlitTexture(m_lBuffer, view_size.x, view_size.y);

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

	m_lFbo->BlitTexture(m_lBuffer, view_size.x, view_size.y);

	m_queryPostProcessing->End();
}

void MyView::DrawEnvironment() {
	glm::mat4 combined_transform = projection_transform * view_transform;

	m_environmentProgram[Program::Instanced]->SetActive();
	m_environmentProgram[Program::Instanced]->BindUniformM4(combined_transform, "combined_transform");
	m_environmentProgram[Program::Instanced]->BindUniformV3(scene_->getAmbientLightIntensity(), "ambience");

	m_instancedVOs->vao.SetActive();
	for (const Mesh& mesh : m_instancedMeshes) {
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.instanceCount, mesh.vertexIndex, mesh.instanceIndex);
	}

	UpdateNonStaticTransforms();
	m_nonStaticVOs->vao.SetActive();
	for (const Mesh& mesh : m_nonStaticMeshes) {
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.instanceCount, mesh.vertexIndex, mesh.instanceIndex);
	}

	m_environmentProgram[Program::NonInstanced]->SetActive();
	m_environmentProgram[Program::NonInstanced]->BindUniformM4(combined_transform, "combined_transform");
	m_environmentProgram[Program::NonInstanced]->BindUniformV3(scene_->getAmbientLightIntensity(), "ambience");

	m_nonInstancedVOs->vao.SetActive();
	GLuint count = m_nonInstancedMeshes.size();
	for (GLuint i = 0; i < count; i++) {
		const Mesh &mesh = m_nonInstancedMeshes[i];

		GLuint model_material = m_nonInstancedVOs->instances[i].material;
		GLint id = glGetUniformLocation(m_environmentProgram[Program::NonInstanced]->getID(), "model.material");
		glUniform1i(id, model_material);

		glm::mat4 model_transform = m_nonInstancedVOs->instances[i].transform;
		m_environmentProgram[Program::NonInstanced]->BindUniformM4(model_transform, "model.transform");

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.vertexIndex);
	}

	VertexArrayObject::Reset();
}

void MyView::DrawLights() {
	m_lightProgram[Light::Directional]->SetActive();
	m_lightVAO[Light::Directional]->SetActive();

	m_lightProgram[Light::Directional]->BindUniformV3(scene_->getCamera().getPosition(), "eyePosition");
	m_lightProgram[Light::Directional]->BindUniformV3(scene_->getAmbientLightIntensity(), "ambience");
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Colour], "colourMap");
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Position], "positionMap", 1);
	m_lightProgram[Light::Directional]->BindUniformTexture(m_gBuffer[GBuffer::Normal], "normalMap", 2);

	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 2);

	m_lightProgram[Light::Point]->SetActive();
	m_lightVAO[Light::Point]->SetActive();

	m_lightProgram[Light::Point]->BindUniformV3(scene_->getCamera().getPosition(), "eyePosition");
	m_lightProgram[Light::Point]->BindUniformV3(scene_->getAmbientLightIntensity(), "ambience");
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Colour], "colourMap");
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Position], "positionMap", 1);
	m_lightProgram[Light::Point]->BindUniformTexture(m_gBuffer[GBuffer::Normal], "normalMap", 2);

	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 20);

	m_lightProgram[Light::Spot]->SetActive();
	m_lightVAO[Light::Spot]->SetActive();

	m_lightProgram[Light::Spot]->BindUniformV3(scene_->getCamera().getPosition(), "eyePosition");
	m_lightProgram[Light::Spot]->BindUniformV3(scene_->getAmbientLightIntensity(), "ambience");
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Colour], "colourMap");
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Position], "positionMap", 1);
	m_lightProgram[Light::Spot]->BindUniformTexture(m_gBuffer[GBuffer::Normal], "normalMap", 2);

	glDrawArraysInstanced(GL_TRIANGLE_FAN, 0, 4, 5);
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
	for (const Mesh &mesh : m_nonStaticMeshes) {
		for (const GLuint &id : scene_->getInstancesByMeshId(mesh.id)) {
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
	m_lightVBO[Light::Directional]->BufferData(dLights);

	std::vector<PointLight> pLights;
	for (const auto &light : scene_->getAllPointLights()) {
		PointLight p;
		p.position = (const glm::vec3&)light.getPosition();
		p.intensity = (const glm::vec3&)light.getIntensity();
		p.range = light.getRange();
		pLights.push_back(p);
	}
	m_lightVBO[Light::Point]->BufferData(pLights);

	std::vector<SpotLight> sLights;
	int i = 0;
	for (const auto &light : scene_->getAllSpotLights()) {
		SpotLight s;
		s.position = (const glm::vec3&)light.getPosition();
		s.direction = (const glm::vec3&)light.getDirection();
		s.intensity = (const glm::vec3&)light.getIntensity();
		s.range = light.getRange();
		s.coneAngle = light.getConeAngleDegrees();
		sLights.push_back(s);
	}
	m_lightVBO[Light::Spot]->BufferData(sLights);
}

#pragma endregion