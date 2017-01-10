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
	struct Shape;
	struct Vertex;
	struct Indirect;
	struct Instance;

	struct StaticVOs;
	struct NonStaticVOs;

	struct DirectionalLight;
	struct PointLight;
	struct SpotLight;
#pragma endregion
#pragma region Members
	#pragma region General
		const scene::Context *scene_;
		GLuint screen_width;
		GLuint screen_height;
		glm::mat4 view_transform;
		glm::mat4 projection_transform;

		PostProcess m_postMode{ PostProcess::Off };
		Mode m_renderMode{ Mode::Forward };
	#pragma endregion
	#pragma region Geometry Objects
		enum GBuffer {
			Colour = 0,
			Position = 1,
			Normal = 2,
			Material = 3
		};

		std::unique_ptr<FrameBufferObject> m_gFbo;
		std::unique_ptr<FrameBufferObject> m_lFbo;
		std::unique_ptr<FrameBufferObject> m_sFbo;
		std::unique_ptr<Texture> m_dbuffer;
		std::unique_ptr<Texture> m_gBuffer[4];
		std::unique_ptr<Texture> m_lBuffer;
	#pragma endregion
	#pragma region Vertex Objects
		std::unique_ptr<StaticVOs> m_staticVOs;
		std::unique_ptr<NonStaticVOs> m_nonStaticVOs;

		std::unique_ptr<Shape> m_lightVO[3];
		std::unique_ptr<VertexBufferObject> m_lightViewVbo;
	#pragma endregion
	#pragma region Materials & Textures
		std::unique_ptr<VertexBufferObject> m_materialUBO;
		std::unique_ptr<Texture> m_mainTexture[7];
		std::unique_ptr<Texture> m_normalTexture[7];
		std::unique_ptr<Texture> m_shadowTexture[5];
	#pragma endregion
	#pragma region Mesh Instances
		std::unique_ptr<Indirect> m_staticMeshes;
		std::unique_ptr<Indirect> m_nonStaticMeshes;
	#pragma endregion
	#pragma region Shader Programs
		std::unique_ptr<ShaderProgram> m_lightProgram[3];
		std::unique_ptr<ShaderProgram> m_environmentProgram;
		std::unique_ptr<ShaderProgram> m_shadowProgram;
	#pragma endregion
	#pragma region Shaders
		std::unique_ptr<Shader> m_vsLight[3];
		std::unique_ptr<Shader> m_fsLight[3];

		std::unique_ptr<Shader> m_vsInstancedEnvironment;
		std::unique_ptr<Shader> m_fsEnvironment;

		std::unique_ptr<Shader> m_vsInstancedShadow;
		std::unique_ptr<Shader> m_fsShadow;
	#pragma endregion
	#pragma region Post-Processing
		std::unique_ptr<PostProcessing> m_antiAliasing;
		std::unique_ptr<PostProcessing> m_celShading;
	#pragma endregion
	#pragma region Time Queries
		std::unique_ptr<TimeQuery> m_queryForwardRender;
		std::unique_ptr<TimeQuery> m_queryDeferredRender;
		std::unique_ptr<TimeQuery> m_queryPostProcessing;
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
	void PrepareVertexData(Indirect &meshData, std::vector<Vertex> &vertices, std::vector<GLuint> &elements, std::vector<Instance> &instances);
#pragma endregion
#pragma region Render Methods
	void ForwardRender();
	void DeferredRender();
	void PostProcessRender();

	void DrawEnvironment();
	void DrawShadows(bool drawStatic = false);
	void DrawLights();
#pragma endregion
#pragma region Additional Methods
	void UpdateViewTransform();
	void UpdateNonStaticTransforms();
	void UpdateLights();
#pragma endregion
};
