#pragma once
#include "Framebuffer.h"
#include "Components/Renderer.h"
#include "Material.h"
#include "Singleton.h"
#include "Mesh.h"
#include "Components/Camera.h"
#include <array>

#include "Shader.h"
#include "Utils/Vertex.h"
#include <glm/glm.hpp>

#include "Components/Light.h"
#include "ShaderLibrary.h"
#include "Components/Skybox.h"

constexpr uint32_t MAX_SCENE_LIGHTS = 64;

class Text;

constexpr uint32_t MAX_RENDER_LAYERS = 10;

class SceneRenderer : public Singleton<SceneRenderer>
{
public:
#pragma region Enumerations
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_TEXT,
		GEO_BALL,
		GEO_BALL2,
		GEO_CUBE,
		GEO_TRIANGLE,
		GEO_QUAD,
		NUM_GEOMETRY,
	};

#pragma endregion Enumerations - Geometry Types - Uniform Types

	void Reset();
	void BeginScene(Camera* sceneCamera, ShaderLibrary* shaderLibrary, Skybox* skybox, const std::vector<LightData*>& lights, uint32_t numLights);
	void BeginScene(const glm::mat4& sceneView,
		const glm::mat4& sceneProjection,
		const glm::vec3& sceneCameraPosition,
		ShaderLibrary* shaderLibrary,
		Skybox* skybox,
		const std::vector<LightData*>& lights,
		uint32_t numLights);

	void BeginScene(Camera* sceneCamera, ShaderLibrary* shaderLibrary);


	void Clear();
	void SetClearColour(const glm::vec4& colour);

	void EndScene();

	void QueueRender(Renderer* r);

	
	void RenderMesh(Mesh* mesh, const glm::mat4& model, bool enableLight = false);
	void RenderMeshOnScreen(Mesh* mesh, const glm::mat4& model);
	void RenderMeshBillboarded(Mesh* mesh, const glm::vec3& worldPosition, const glm::vec2& worldScale);
	void RenderText(Text* text);
	void RenderTextOnScreen(Text* text);

	void OnWindowResize(uint32_t width, uint32_t height);

	void Flush();

	Mesh* GetMesh(GEOMETRY_TYPE t) const;

	const std::array<Mesh*, NUM_GEOMETRY> GetMeshList(void) const;

	SceneRenderer();
	virtual ~SceneRenderer();
	
	void DrawIndexed(Mesh* mesh, uint32_t indexCount);
	void DrawIndexed(Mesh* mesh, uint32_t indexCount = 0, uint32_t offset = 0);
private:
	// TODO: Make it named
	// Lower = first.
	std::array<std::vector<Renderer*>, MAX_RENDER_LAYERS> renderLayers_;

	Shader* billboardShader_;

	glm::vec3 cameraPosition_;

	glm::mat4 orthoProjection_;
	glm::mat4 canvasView_;

	glm::mat4 sceneViewMatrix_;
	glm::mat4 sceneProjectionMatrix_;

	//Framebuffer objectFramebuffer;

	ShaderLibrary* sceneShaderLibrary_;
	Camera* refCamera_;
	Skybox* refSkybox_;

	std::array<LightData, MAX_SCENE_LIGHTS> lightBlock_{};

	std::array<Mesh*, NUM_GEOMETRY> meshList_;

	uint32_t numLights_;

	unsigned int vertexArrayID_;


};

