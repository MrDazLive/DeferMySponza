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
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height) {
    glViewport(0, 0, width, height);

	const float aspect_ratio = width / height;
	projection_transform = glm::perspective(1.31f, aspect_ratio, 1.f, 1000.f);

	delete m_fbo;
	for (Texture *ptr : m_gbuffer) {
		delete ptr;
	}

	PrepareGBs(width, height);
}

void MyView::windowViewDidStop(tygra::Window * window) {
	delete m_fbo;
	for (Texture *ptr : m_gbuffer) {
		delete ptr;
	}

	delete m_instancedVOs;
	delete m_nonStaticVOs;
	delete m_nonInstancedVOs;

	delete m_materialUBO;
	for (Texture *ptr : m_mainTexture) {
		delete ptr;
	}

	delete m_instancedProgram;
	delete m_nonInstancedProgram;

	delete m_instancedVS;
	delete m_nonInstancedVS;
	delete m_meshFS;

	delete m_queryFullDraw;
	delete m_queryInstancedDraw;
	delete m_queryMovingDraw;
	delete m_queryUniqueDraw;
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
		break;
	default:
		std::cerr << "No valid render mode selected." << std::endl;
	}
}

#pragma endregion
#pragma region Additional Methods

void MyView::LogTimers() {
	std::cout << "Environemnt Draw: (" << m_queryFullDraw->toString() << ")" << std::endl;
	std::cout << "Instanced Draw: (" << m_queryInstancedDraw->toString() << ")" << std::endl;
	std::cout << "Moving Draw: (" << m_queryMovingDraw->toString() << ")" << std::endl;
	std::cout << "Unique Draw: (" << m_queryUniqueDraw->toString() << ")" << std::endl;
	std::cout << std::endl;
}

void MyView::ResetTimers() {
	m_queryFullDraw->Reset();
	m_queryInstancedDraw->Reset();
	m_queryMovingDraw->Reset();
	m_queryUniqueDraw->Reset();
}

void MyView::ReloadShaders() {
	delete m_instancedVS;
	delete m_nonInstancedVS;
	delete m_meshFS;
	PrepareShaders();
	if ((m_nonInstancedVS->getStatus() && m_instancedVS->getStatus() && m_meshFS->getStatus()) == GL_TRUE) {
		delete m_instancedProgram;
		delete m_nonInstancedProgram;
		PreparePrograms();
	}
}

#pragma endregion
#pragma region Setup Methods

void MyView::PrepareVOs() {
	m_instancedVOs = new InstanceVOs();
	m_nonStaticVOs = new NonStaticVOs();
	m_nonInstancedVOs = new NonInstanceVOs();

	PrepareMeshData();

	PrepareVAOs();
	PrepareVBOs();
	PrepareUBOs();

	PrepareShaders();
	PreparePrograms();

	PrepareTextures();
}

void MyView::PrepareGBs(const float width, const float height) {
	m_fbo = new FrameBufferObject();

	for (unsigned int i = 0; i < 4; i++) {
		m_gbuffer[i] = new Texture(GL_TEXTURE_2D);
		m_gbuffer[i]->Buffer(GL_RGB32F, width, height, GL_RGB, GL_FLOAT);
		m_fbo->AttachTexture(GL_COLOR_ATTACHMENT0 + i, m_gbuffer[i]);
	}

	GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, drawBuffers);

	m_fbo->LogInfo();
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
}

void MyView::PrepareUBOs() {
	m_materialUBO = new VertexBufferObject(GL_UNIFORM_BUFFER, GL_STATIC_DRAW);
	m_materialUBO->BindRange(0, 0, sizeof(scene::Material) * 7);
	m_materialUBO->BufferData(scene_->getAllMaterials()[0], 7);
}

void MyView::PrepareTimers() {
	m_queryFullDraw = new TimeQuery();
	m_queryInstancedDraw = new TimeQuery();
	m_queryMovingDraw = new TimeQuery();
	m_queryUniqueDraw = new TimeQuery();
}

void MyView::PrepareShaders() {
	m_instancedVS = new Shader(GL_VERTEX_SHADER);
	m_instancedVS->LoadFile("resource:///instanced_vs.glsl");

	m_nonInstancedVS = new Shader(GL_VERTEX_SHADER);
	m_nonInstancedVS->LoadFile("resource:///nonInstanced_vs.glsl");

	m_meshFS = new Shader(GL_FRAGMENT_SHADER);
	m_meshFS->LoadFile("resource:///mesh_fs.glsl");
}

void MyView::PreparePrograms() {
	m_instancedProgram = new ShaderProgram();

	m_instancedProgram->AddShader(m_instancedVS);
	m_instancedProgram->AddShader(m_meshFS);

	m_instancedProgram->AddInAttribute("vertex_position");
	m_instancedProgram->AddInAttribute("vertex_normal");
	m_instancedProgram->AddInAttribute("vertex_tangent");
	m_instancedProgram->AddInAttribute("vertex_texture_coordinate");

	m_instancedProgram->AddInAttribute("model");

	m_instancedProgram->AddOutAttribute("fragment_colour");

	m_instancedProgram->Link();

	m_instancedProgram->BindBlock(m_materialUBO, "block_material");

	m_nonInstancedProgram = new ShaderProgram();

	m_nonInstancedProgram->AddShader(m_nonInstancedVS);
	m_nonInstancedProgram->AddShader(m_meshFS);

	m_nonInstancedProgram->AddInAttribute("vertex_position");
	m_nonInstancedProgram->AddInAttribute("vertex_normal");
	m_nonInstancedProgram->AddInAttribute("vertex_tangent");
	m_nonInstancedProgram->AddInAttribute("vertex_texture_coordinate");

	m_nonInstancedProgram->AddOutAttribute("fragment_colour");

	m_nonInstancedProgram->Link();

	m_nonInstancedProgram->BindBlock(m_materialUBO, "block_material");
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

	std::cout << "Instanced Meshes: " << m_instancedMeshes.size() << " || ";
	std::cout << "Unique Meshes: " << m_nonInstancedMeshes.size() << " || ";
	std::cout << "Non Static Meshes: " << m_nonStaticMeshes.size() << std::endl;
	std::cout << std::endl;
}

void MyView::PrepareTextures() {
	GLuint mainTextureID[7];
	std::string mainTexture[7] = { "content:///brick.png", "content:///wall.png", "content:///not_fabric.png" };
	GLuint normalTextureID[7];
	std::string normalTexture[7] = { "content:///brick_norm.png", "content:///wall_norm.png", "content:///not_fabric_norm.png" };
	
	for (unsigned int i = 0; i < 7; i++) {
		if (mainTexture[i].size() > 0) {
			m_mainTexture[i] = new Texture(GL_TEXTURE_2D);
			m_mainTexture[i]->LoadFile(mainTexture[i]);
			mainTextureID[i] = m_mainTexture[i]->getID();
		}

		if (normalTexture[i].size() > 0) {
			m_normalTexture[i] = new Texture(GL_TEXTURE_2D);
			m_normalTexture[i]->LoadFile(normalTexture[i]);
			normalTextureID[i] = m_normalTexture[i]->getID();
		}
	}

	m_instancedProgram->SetActive();
	for (unsigned int i = 0, j = 0; i < 7; i++, j++) {
		std::string main = "mainTexture[" + std::to_string(i) + "]";
		std::string normal = "normalTexture[" + std::to_string(i) + "]";

		glActiveTexture(GL_TEXTURE0 + j);
		glBindTexture(GL_TEXTURE_2D, mainTextureID[i]);
		GLuint main_id = glGetUniformLocation(m_instancedProgram->getID(), main.c_str());
		glUniform1i(main_id, j);

		j++;

		glActiveTexture(GL_TEXTURE0 + j);
		glBindTexture(GL_TEXTURE_2D, normalTextureID[i]);
		GLuint normal_id = glGetUniformLocation(m_instancedProgram->getID(), normal.c_str());
		glUniform1i(normal_id, j);
	}

	m_nonInstancedProgram->SetActive();
	for (unsigned int i = 0, j = 0; i < 7; i++, j++) {
		std::string main = "mainTexture[" + std::to_string(i) + "]";
		std::string normal = "normalTexture[" + std::to_string(i) + "]";

		glActiveTexture(GL_TEXTURE0 + j);
		glBindTexture(GL_TEXTURE_2D, mainTextureID[i]);
		GLuint main_id = glGetUniformLocation(m_nonInstancedProgram->getID(), main.c_str());
		glUniform1i(main_id, j);

		j++;

		glActiveTexture(GL_TEXTURE0 + j);
		glBindTexture(GL_TEXTURE_2D, normalTextureID[i]);
		GLuint normal_id = glGetUniformLocation(m_nonInstancedProgram->getID(), normal.c_str());
		glUniform1i(normal_id, j);
	}
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	//m_queryFullDraw->Begin();
	ForwardRenderEnvironment();
	//m_queryFullDraw->End();

	VertexArrayObject::Reset();
}

void MyView::ForwardRenderEnvironment() {
	glm::mat4 combined_transform = projection_transform * view_transform;

	m_instancedProgram->SetActive();
	m_instancedProgram->BindUniformV3(scene_->getCamera().getPosition(), "camera_position");
	m_instancedProgram->BindUniformM4(combined_transform, "combined_transform");

	m_queryInstancedDraw->Begin();
	m_instancedVOs->vao.SetActive();
	for (const Mesh& mesh : m_instancedMeshes) {
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.instanceCount, mesh.vertexIndex, mesh.instanceIndex);
	}
	m_queryInstancedDraw->End();

	m_queryMovingDraw->Begin();
	UpdateNonStaticTransforms();
	m_nonStaticVOs->vao.SetActive();
	for (const Mesh& mesh : m_nonStaticMeshes) {
		glDrawElementsInstancedBaseVertexBaseInstance(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.instanceCount, mesh.vertexIndex, mesh.instanceIndex);
	}
	m_queryMovingDraw->End();

	m_nonInstancedProgram->SetActive();
	m_nonInstancedProgram->BindUniformV3(scene_->getCamera().getPosition(), "camera_position");
	m_nonInstancedProgram->BindUniformM4(combined_transform, "combined_transform");

	m_queryUniqueDraw->Begin();
	m_nonInstancedVOs->vao.SetActive();
	GLuint count = m_nonInstancedMeshes.size();
	for (GLuint i = 0; i < count; i++) {
		const Mesh &mesh = m_nonInstancedMeshes[i];

		GLuint model_material = m_nonInstancedVOs->instances[i].material;
		GLint id = glGetUniformLocation(m_nonInstancedProgram->getID(), "model_material");
		glUniform1i(id, model_material);

		glm::mat4 model_transform = m_nonInstancedVOs->instances[i].transform;
		m_nonInstancedProgram->BindUniformM4(model_transform, "model_transform");

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.vertexIndex);
	}
	m_queryUniqueDraw->End();
}

void MyView::DeferredRender() {
	m_fbo->SetDraw();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);

	DeferredRenderEnvironment();

	FrameBufferObject::Reset();
}

void MyView::DeferredRenderEnvironment() {

	ForwardRenderEnvironment();
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

#pragma endregion