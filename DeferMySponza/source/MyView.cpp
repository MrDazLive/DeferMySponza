#include "MyView.hpp"

#include <scene/scene.hpp>
#include <tygra/FileHelper.hpp>
#include <tsl/shapes.hpp>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cassert>

#include "rendering\Shader.h"
#include "rendering\ShaderProgram.h"
#include "rendering\VertexArrayObject.h"
#include "rendering\VertexBufferObject.h"

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
	glm::vec2 textureCoordinate;
};

struct MyView::InstanceVOs {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject<GLuint> element_vbo = VertexBufferObject<GLuint>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject<Vertex> vertex_vbo = VertexBufferObject<Vertex>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject<glm::mat4> instance_vbo = VertexBufferObject<glm::mat4>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
};

struct MyView::NonStaticVOs {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject<GLuint> element_vbo = VertexBufferObject<GLuint>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject<Vertex> vertex_vbo = VertexBufferObject<Vertex>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject<glm::mat4> instance_vbo = VertexBufferObject<glm::mat4>(GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);

	std::vector<glm::mat4> instances;
};

struct MyView::NonInstanceVOs {
	VertexArrayObject vao = VertexArrayObject();
	VertexBufferObject<GLuint> element_vbo = VertexBufferObject<GLuint>(GL_ELEMENT_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject<Vertex> vertex_vbo = VertexBufferObject<Vertex>(GL_ARRAY_BUFFER, GL_STATIC_DRAW);
	VertexBufferObject<glm::mat4> instance_vbo = VertexBufferObject<glm::mat4>(GL_UNIFORM_BUFFER, GL_STATIC_DRAW);
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

#pragma endregion
#pragma region Window Methods

void MyView::windowViewWillStart(tygra::Window * window) {
    assert(scene_ != nullptr);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_ONE, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);

	PrepareVOs();
}

void MyView::windowViewDidReset(tygra::Window * window,
                                int width,
                                int height) {
    glViewport(0, 0, width, height);

	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	const float aspect_ratio = viewport[2] / (float)viewport[3];
	projection_transform = glm::perspective(1.31f, aspect_ratio, 1.f, 1000.f);
}

void MyView::windowViewDidStop(tygra::Window * window) {
	delete m_instancedVOs;
	delete m_nonStaticVOs;
	delete m_nonInstancedVOs;

	delete m_instancedProgram;
	delete m_nonInstancedProgram;

	delete m_instancedVS;
	delete m_nonInstancedVS;
	delete m_meshFS;
}

void MyView::windowViewRender(tygra::Window * window) {
    assert(scene_ != nullptr);

	glDepthMask(GL_TRUE);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClearColor(0.f, 0.f, 0.25f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	scene::Camera camera = scene_->getCamera();
	glm::vec3 camera_pos = (const glm::vec3&)camera.getPosition();
	glm::vec3 camera_dir = (const glm::vec3&)camera.getDirection();
	glm::mat4 view_xform = glm::lookAt(camera_pos, camera_pos + camera_dir, glm::vec3(0, 1, 0));
	glm::mat4 combined_xform = projection_transform * view_xform;

	m_instancedProgram->SetActive();
	m_instancedProgram->BindUniform(glUniformMatrix4fv, combined_xform, "combined_xform");

	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glDisable(GL_BLEND);
	m_instancedVOs->vao.SetActive();
	for (const Mesh& mesh : m_instancedMeshes) {
		//m_instancedVOs->instance_vbo.BindRange(mesh.instanceCount, mesh.instanceIndex);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.instanceCount, mesh.vertexIndex);
	}

	UpdateNonStaticTransforms();
	m_nonStaticVOs->vao.SetActive();
	for (const Mesh& mesh : m_nonStaticMeshes) {
		//m_nonStaticVOs->instance_vbo.BindRange(mesh.instanceCount, mesh.instanceIndex);
		glDrawElementsInstancedBaseVertex(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.instanceCount, mesh.vertexIndex);
	}

	m_nonInstancedProgram->SetActive();
	m_nonInstancedProgram->BindUniform(glUniformMatrix4fv, combined_xform, "combined_xform");
	m_nonInstancedVOs->vao.SetActive();
	for (const Mesh &mesh : m_nonInstancedMeshes) {
		const GLuint id = scene_->getInstancesByMeshId(mesh.id)[0];
		const auto& instance = scene_->getInstanceById(id);

		glm::mat4 model_transform = (const glm::mat4x3&)instance.getTransformationMatrix();
		GLuint model_transform_id = glGetUniformLocation(m_nonInstancedProgram->getID(), "model_transform");
		glUniformMatrix4fv(model_transform_id, 1, GL_FALSE, glm::value_ptr(model_transform));

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh.elementCount, GL_UNSIGNED_INT, (GLintptr*)(mesh.elementIndex * sizeof(GLuint)), mesh.vertexIndex);
	}

	ShaderProgram::Reset();
	VertexArrayObject::Reset();
}

#pragma endregion
#pragma region Additional Methods

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

	PrepareShaders();
	PreparePrograms();
}

void MyView::PrepareVAOs() {
	m_instancedVOs->vao.SetActive();
	m_instancedVOs->element_vbo.SetActive();
	m_instancedVOs->vertex_vbo.SetActive();
	m_instancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_instancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_instancedVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_instancedVOs->instance_vbo.SetActive();
	m_instancedVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE);
	m_instancedVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4)));
	m_instancedVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 2));
	m_instancedVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 3));

	m_nonStaticVOs->vao.SetActive();
	m_nonStaticVOs->element_vbo.SetActive();
	m_nonStaticVOs->vertex_vbo.SetActive();
	m_nonStaticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_nonStaticVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_nonStaticVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));
	m_nonStaticVOs->instance_vbo.SetActive();
	m_nonStaticVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE);
	m_nonStaticVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4)));
	m_nonStaticVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 2));
	m_nonStaticVOs->vao.AddAttributeDivisor<glm::mat4>(4, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec4) * 3));

	m_nonInstancedVOs->vao.SetActive();
	m_nonInstancedVOs->element_vbo.SetActive();
	m_nonInstancedVOs->vertex_vbo.SetActive();
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE);
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(3, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3)));
	m_nonInstancedVOs->vao.AddAttribute<Vertex>(2, GL_FLOAT, GL_FALSE, (int*)(sizeof(glm::vec3) * 2));

	VertexArrayObject::Reset();
	VertexBuffer::Reset(GL_ARRAY_BUFFER);
	VertexBuffer::Reset(GL_ELEMENT_ARRAY_BUFFER);
}

void MyView::PrepareVBOs() {
	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;
	std::vector<glm::mat4> instances;

	PrepareVertexData(m_instancedMeshes, vertices, elements, instances);
	m_instancedVOs->vertex_vbo.setData(&vertices[0]);
	m_instancedVOs->vertex_vbo.setSize(vertices.size());
	m_instancedVOs->vertex_vbo.BufferData();
	m_instancedVOs->element_vbo.setData(&elements[0]);
	m_instancedVOs->element_vbo.setSize(elements.size());
	m_instancedVOs->element_vbo.BufferData();
	m_instancedVOs->instance_vbo.setData(&instances[0]);
	m_instancedVOs->instance_vbo.setSize(instances.size());
	m_instancedVOs->instance_vbo.BufferData();
	vertices.clear();
	elements.clear();
	instances.clear();

	PrepareVertexData(m_nonStaticMeshes, vertices, elements, instances);
	m_nonStaticVOs->vertex_vbo.setData(&vertices[0]);
	m_nonStaticVOs->vertex_vbo.setSize(vertices.size());
	m_nonStaticVOs->vertex_vbo.BufferData();
	m_nonStaticVOs->element_vbo.setData(&elements[0]);
	m_nonStaticVOs->element_vbo.setSize(elements.size());
	m_nonStaticVOs->element_vbo.BufferData();
	m_nonStaticVOs->instances = instances;
	m_nonStaticVOs->instance_vbo.setData(&m_nonStaticVOs->instances[0]);
	m_nonStaticVOs->instance_vbo.setSize(m_nonStaticVOs->instances.size());
	m_nonStaticVOs->instance_vbo.BufferData();
	vertices.clear();
	elements.clear();
	instances.clear();

	PrepareVertexData(m_nonInstancedMeshes, vertices, elements, instances);
	m_nonInstancedVOs->vertex_vbo.setData(&vertices[0]);
	m_nonInstancedVOs->vertex_vbo.setSize(vertices.size());
	m_nonInstancedVOs->vertex_vbo.BufferData();
	m_nonInstancedVOs->element_vbo.setData(&elements[0]);
	m_nonInstancedVOs->element_vbo.setSize(elements.size());
	m_nonInstancedVOs->element_vbo.BufferData();
	m_nonInstancedVOs->instance_vbo.setData(&m_nonStaticVOs->instances[0]);
	m_nonInstancedVOs->instance_vbo.setSize(m_nonStaticVOs->instances.size());
	m_nonInstancedVOs->instance_vbo.BufferData();
	vertices.clear();
	elements.clear();
	instances.clear();
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
	m_instancedProgram->AddInAttribute("vertex_texture_coordinate");
	m_instancedProgram->AddInAttribute("model_transform");

	m_instancedProgram->AddOutAttribute("fragment_colour");

	m_instancedProgram->Link();

	m_nonInstancedProgram = new ShaderProgram();

	m_nonInstancedProgram->AddShader(m_nonInstancedVS);
	m_nonInstancedProgram->AddShader(m_meshFS);

	m_nonInstancedProgram->AddInAttribute("vertex_position");
	m_nonInstancedProgram->AddInAttribute("vertex_normal");
	m_nonInstancedProgram->AddInAttribute("vertex_texture_coordinate");

	m_nonInstancedProgram->AddOutAttribute("fragment_colour");

	m_nonInstancedProgram->Link();
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

	std::cout << "Instanced Meshes:  " << m_instancedMeshes.size() << std::endl;
	std::cout << "Separate Meshes:   " << m_nonInstancedMeshes.size() << std::endl;
	std::cout << "Non Static Meshes: " << m_nonStaticMeshes.size() << std::endl;
	std::cout << std::endl;
}

void MyView::PrepareVertexData(std::vector<Mesh> &meshData, std::vector<Vertex> &vertices, std::vector<GLuint> &elements, std::vector<glm::mat4> &instances) {
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
		const auto &textureCoordinates = mesh.getTextureCoordinateArray();

		const unsigned int vertexCount = positions.size();

		if (textureCoordinates.size() == 0) {
			for (unsigned int i = 0; i < vertexCount; i++) {
				Vertex v;
				v.positiion = (const glm::vec3&)positions[i];
				v.normal = (const glm::vec3&)normals[i];
				v.textureCoordinate = glm::vec2(0);
				vertices.push_back(v);
			}
		} else {
			for (unsigned int i = 0; i < vertexCount; i++) {
				Vertex v;
				v.positiion = (const glm::vec3&)positions[i];
				v.normal = (const glm::vec3&)normals[i];
				v.textureCoordinate = (const glm::vec2&)textureCoordinates[i];
				vertices.push_back(v);
			}
		}

		const auto &meshInstances = scene_->getInstancesByMeshId(m.id);
		for (const auto &instanceID : meshInstances) {
			const auto &instance = scene_->getInstanceById(instanceID);
			instances.push_back(glm::mat4((const glm::mat4x3&)instance.getTransformationMatrix()));
		}
		m.instanceCount = meshInstances.size();
	}
}

#pragma endregion
#pragma region Additional Methods

void MyView::UpdateNonStaticTransforms() {
	m_nonStaticVOs->instances.clear();
	for (const Mesh &mesh : m_nonStaticMeshes) {
		for (const GLuint &id : scene_->getInstancesByMeshId(mesh.id)) {
			const auto &instance = scene_->getInstanceById(id);
			m_nonStaticVOs->instances.push_back((const glm::mat4x3&)instance.getTransformationMatrix());
		}
	}
	m_nonStaticVOs->instance_vbo.BufferData();
}

#pragma endregion