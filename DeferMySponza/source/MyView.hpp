#pragma once

#include <scene/scene_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "rendering\enums\Mode.h"

class VertexBufferObject;
class VertexArrayObject;
class FrameBufferObject;
class ShaderProgram;
class TimeQuery;
class Texture;
class Shader;

class MyView : public tygra::WindowViewDelegate
{
public:
#pragma region Constructors/Destructors
    MyView();
    ~MyView();
#pragma endregion
#pragma region Getters/Setters
    void setScene(const scene::Context * scene);
	void setMode(const Mode mode);
#pragma endregion
#pragma region Additional Methods
	void LogTimers();
	void ResetTimers();
	void ReloadShaders();
#pragma endregion
private:
#pragma region Structs
	struct Mesh;
	struct Vertex;
	struct Instance;

	struct InstanceVOs;
	struct NonStaticVOs;
	struct NonInstanceVOs;
#pragma endregion
#pragma region Members
	#pragma region General
		const scene::Context *scene_;
		glm::mat4 view_transform;
		glm::mat4 projection_transform;

		Mode m_renderMode{ Mode::Deferred };

		FrameBufferObject *m_fbo;
	#pragma endregion
	#pragma region Vertex Objects
		InstanceVOs *m_instancedVOs;
		NonStaticVOs *m_nonStaticVOs;
		NonInstanceVOs *m_nonInstancedVOs;
	#pragma endregion
	#pragma region Materials & Textures
		VertexBufferObject *m_materialUBO;
		Texture *m_mainTexture[7];
		Texture *m_normalTexture[7];
	#pragma endregion
	#pragma region Mesh Instances
		std::vector<Mesh> m_instancedMeshes;
		std::vector<Mesh> m_nonStaticMeshes;
		std::vector<Mesh> m_nonInstancedMeshes;
	#pragma endregion
	#pragma region Shader Programs
		ShaderProgram *m_instancedProgram;
		ShaderProgram *m_nonInstancedProgram;
	#pragma endregion
	#pragma region Shaders
		Shader *m_instancedVS;
		Shader *m_nonInstancedVS;
		Shader *m_meshFS;
	#pragma endregion
	#pragma region Time Queries
		TimeQuery *m_queryFullDraw;
		TimeQuery *m_queryInstancedDraw;
		TimeQuery *m_queryMovingDraw;
		TimeQuery *m_queryUniqueDraw;
	#pragma endregion
#pragma endregion
#pragma region Window Methods
    void windowViewWillStart(tygra::Window * window) override;

    void windowViewDidReset(tygra::Window * window,
                            int width,
                            int height) override;

    void windowViewDidStop(tygra::Window * window) override;

    void windowViewRender(tygra::Window * window) override;
#pragma endregion
#pragma region Setup Methods
	void PrepareVOs();
	void PrepareFBO();
	void PrepareVAOs();
	void PrepareVBOs();
	void PrepareUBOs();
	void PrepareTimers();
	void PrepareShaders();
	void PreparePrograms();
	void PrepareMeshData();
	void PrepareTextures();
	void PrepareVertexData(std::vector<Mesh> &meshData, std::vector<Vertex> &vertices, std::vector<GLuint> &elements, std::vector<Instance> &instances);
#pragma endregion
#pragma region Render Methods
	void ForwardRender();
	void ForwardRenderEnvironment();

	void DeferredRender();
	void DeferredRenderEnvironment();
#pragma endregion
#pragma region Additional Methods
	void UpdateViewTransform();
	void UpdateNonStaticTransforms();
#pragma endregion
};
