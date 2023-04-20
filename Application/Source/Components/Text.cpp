#include "Text.h"
#include "Core/Log.h"
#include <glad/glad.h>
#include "Renderer/SceneRenderer.h"
#include "Renderer/MeshBuilder.h"
#include "Core/Application.h"
#include "Scene/GameObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include "Utils/Font.h"

#define TEXT_ROWS 16
#define TEXT_COLS 16	

Text::Text(GameObject* go)
	: Renderer(go)
	, targetTransform_(go->GetTransform())
	, text_("Text")
	, colour({1, 1, 1})
	, font(nullptr)
	, enableDialogueBox_(false)
{
	// Probably need to check whether there is already a renderer.
	go->SetRenderer(this);
	textMesh_ = MeshBuilder::GenerateText("Text", TEXT_ROWS, TEXT_COLS);
	textShader_ = ShaderLibrary::GetInstance()->Get("Text");
	renderOrder = 1;
}

Text::~Text()
{
	delete textMesh_;
	textMesh_ = nullptr;
}

void Text::Render()
{
	//const auto& position = _targetTransform->GetPosition();

	if (!onScreen)
		SceneRenderer::GetInstance()->RenderText(this);
	else
		SceneRenderer::GetInstance()->RenderTextOnScreen(this);
}

void Text::Update(double dt)
{
}

void Text::SetText(const std::string& text)
{
	text_ = text;
}

const std::string& Text::GetText() const
{
	return text_;
}

void Text::EnableDialogueBox(bool enable)
{
	dialogueBox_->GetRenderer()->SetActive(enable);
}


void Text::Render(const glm::mat4& view, const glm::mat4& projection)
{
	if (!font || !font->fontTexture)
		return;
	const auto& shader = textShader_;
	shader->Bind();
	glDisable(GL_DEPTH_TEST);

	auto model = targetTransform_->GetTransformMatrix();

	shader->SetFloat3("u_textColour", colour);
	font->fontTexture->Bind(15);
	shader->SetInt("u_textTexture", 15);
	
	static SceneRenderer* renderer = SceneRenderer::GetInstance();

	for (unsigned i = 0; i < text_.length(); ++i)
	{
		glm::mat4 characterSpacing;
		characterSpacing = glm::translate(glm::mat4(1.0f), glm::vec3((i * font->fontSpacing[i] * font->fontMultiplier) + 0.5f, 0.5f, 0)); //1.0f is the spacing of each character, you may change this value
		glm::mat4 MVP = projection * view * model * characterSpacing;
		shader->SetMat4("MVP", MVP);
		//textMesh_->Render((unsigned int)text_[i] * 6, 6);
		renderer->DrawIndexed(textMesh_, 6, static_cast<uint32_t>(text_[i]) * 6);
	}

	font->fontTexture->Unbind();

	glEnable(GL_DEPTH_TEST);
}

void Text::BindDialogueBox(std::shared_ptr<GameObject> db, bool enable)
{
	dialogueBox_ = db;
	db->GetRenderer()->SetActive(false);
	enableDialogueBox_ = enable;
	isDirty_ = true;
}
