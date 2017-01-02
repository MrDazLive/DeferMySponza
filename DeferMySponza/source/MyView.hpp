#pragma once

#include <scene/scene_fwd.hpp>
#include <tygra/WindowViewDelegate.hpp>
#include <tgl/tgl.h>
#include <glm/glm.hpp>

#include <vector>
#include <memory>

#include "rendering\enums\Mode.h"
#include "rendering\enums\PostProcess.h"

class VertexBufferObject;
class VertexArrayObject;
class FrameBufferObject;
class PostProcessing;
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
	void TogglePostProcessing();
#pragma endregion
private:
#pragma region Structs
	struct Mesh;
	struct Vertex;
	struct Instance;

	struct InstanceVOs;
	struct NonStaticVOs;
	struct NonInstanceVOs;

	struct DirectionalLight;
	struct PointLight;
	struct SpotLight;
#pragma endregion
#pragma region Members
	#pragma region General
		const scene::Context *scene_;
		glm::vec2 view_size;
		glm::mat4 view_transform;
		glm::mat4 projection_transform;

		PostProcess m_postMode{ PostProcess::Off };
		Mode m_renderMode{ Mode::Deferred };
	#pragma endregion
	#pragma region Geometry Objects
		enum GBuffer {
			Colour = 0,
			Position = 1,
			Normal = 2,
			Material = 3
		};

		FrameBufferObject *m_gFbo{ nullptr };
		FrameBufferObject *m_lFbo{ nullptr };
		Texture *m_dbuffer{ nullptr };
		Texture *m_gBuffer[4]{ nullptr };
		Texture *m_lBuffer{ nullptr };
	#pragma endregion
	#pragma region Vertex Objects
		InstanceVOs *m_instancedVOs{ nullptr };
		NonStaticVOs *m_nonStaticVOs{ nullptr };
		NonInstanceVOs *m_nonInstancedVOs{ nullptr };

		VertexArrayObject *m_lightVAO[3]{ nullptr };
		VertexBufferObject *m_quadVBO{ nullptr };
	#pragma endregion
	#pragma region Materials & Textures
		VertexBufferObject *m_materialUBO{ nullptr };
		Texture *m_mainTexture[7]{ nullptr };
		Texture *m_normalTexture[7]{ nullptr };
	#pragma endregion
	#pragma region Lights
		enum Light {
			Directional = 0,
			Point = 1,
			Spot = 2
		};
		VertexBufferObject *m_lightVBO[3]{ nullptr };
	#pragma endregion
	#pragma region Mesh Instances
		std::vector<Mesh> m_instancedMeshes;
		std::vector<Mesh> m_nonStaticMeshes;
		std::vector<Mesh> m_nonInstancedMeshes;
	#pragma endregion
	#pragma region Shader Programs
		enum Program {
			Instanced = 0,
			NonInstanced = 1
		};

		ShaderProgram *m_lightProgram[3]{ nullptr };
		ShaderProgram *m_environmentProgram[2]{ nullptr };
	#pragma endregion
	#pragma region Shaders
		Shader *m_vsLight{ nullptr };
		Shader *m_fsLight[3]{ nullptr };

		Shader *m_vsInstanced{ nullptr };
		Shader *m_vsNonInstanced{ nullptr };
		Shader *m_fsEnvironment{ nullptr };
	#pragma endregion
	#pragma region Post-Processing
		PostProcessing *m_antiAliasing;
		PostProcessing *m_celShading;
	#pragma endregion
	#pragma region Time Queries
		TimeQuery *m_queryForwardRender{ nullptr };
		TimeQuery *m_queryDeferredRender{ nullptr };
		TimeQuery *m_queryPostProcessing{ nullptr };
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
	void PrepareGBs();
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
	void DeferredRender();
	void PostProcessRender();

	void DrawEnvironment();
	void DrawLights();
#pragma endregion
#pragma region Additional Methods
	void UpdateViewTransform();
	void UpdateNonStaticTransforms();
	void UpdateLights();
#pragma endregion
};
