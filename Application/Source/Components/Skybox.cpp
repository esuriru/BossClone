
#include "Utils/Util.h"
#include "Core/Application.h"
#include <exception>
#include "Renderer/MeshBuilder.h"
#include "Skybox.h"
#include "Utils/LoadTGA.h"
#include "Renderer/SceneRenderer.h"

//ShaderLibrary Skybox::skyboxShaderLibrary_;

Skybox::Skybox(std::vector<std::string> faces)
	: textureID_(LoadTGACubemap(faces))
	, box_(MeshBuilder::GenerateCube("Skybox", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)))
	, rotationMatrix_(1.0f)
{
	if (faces.size() != 6)
		throw std::invalid_argument("Too many or too less faces.");
	//skyboxShader_ = skyboxShaderLibrary_.Get("skybox proprietary shader");
	skyboxShader_ = ShaderLibrary::GetInstance()->Get("skybox proprietary shader");
}

Skybox::~Skybox()
{
	if (box_)
	{
		delete box_;
		box_ = nullptr;
	}

	glDeleteTextures(1, &textureID_);
    //delete _framebuffer;
}

void Skybox::Bind(uint32_t slot)
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID_);
}

void Skybox::Unbind()
{
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glActiveTexture(GL_TEXTURE0);
}

void Skybox::Render(const glm::mat4& camera_v, const glm::mat4& p)
{
	const auto& shader = skyboxShader_;
	shader->Bind();

	GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
	
	//static float rotation = 0;
	//rotation += 0.05f;
	//if (rotation > 360)
	//	rotation -= 360.f;

	//constexpr glm::vec3 rotation_direction = glm::vec3(Util::sqrt(2), Util::sqrt(2), 0);
	const auto& view = glm::mat4(glm::mat3(camera_v * rotationMatrix_));
	shader->SetMat4("view", view);
	shader->SetMat4("projection", p);

	Bind();
    shader->SetInt("skybox", 11);

    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);

		
	
	static SceneRenderer* sr = SceneRenderer::GetInstance();
	sr->DrawIndexed(box_);

	Unbind();
       
    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);
}

void Skybox::SetRotation(glm::mat4 rotation)
{
	rotationMatrix_ = rotation;
}
