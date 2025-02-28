//
// Created by QuzzS on 2025/2/28.
//


#include "Test_Scene.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

void TestScene::Start()
{
	LOG_DEBUG("Entering Test Scene");
	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(0);

	m_Title->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/Title.png"));
	m_Title->SetZIndex(2);
	m_Title->SetPivot({234,-221.5});

	m_RedShawl->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/RedShawl.png"));
	m_RedShawl->SetZIndex(1);
	m_RedShawl->SetPivot({-237,-22});

	m_Character->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/pet00icon.png"));
	m_Character->SetZIndex(10);

	m_Text->Init( 20, 2, glm::vec2(-11,300) );
	m_Version->Init( 20, 2, glm::vec2(-451,300) );

	m_Root.AddChild(m_Background);
	m_Root.AddChild(m_RedShawl);
	m_Root.AddChild(m_Title);
	m_Root.AddChild(m_Version);
	m_Root.AddChild(m_Text);
	m_Root.AddChild(m_Character);

	m_Camera.AddChild(m_Background);
	m_Camera.AddChild(m_RedShawl);
	m_Camera.AddChild(m_Title);
	m_Camera.AddChild(m_Version);
	m_Camera.AddChild(m_Text);
	m_Camera.AddChild(m_Character);
}

void TestScene::Input()
{
}

void TestScene::Update()
{
	//LOG_DEBUG("Test Scene is running...");
	glm::vec2 direction = {0.0f,0.0f};
	if (Util::Input::IsKeyPressed(Util::Keycode::W)) {direction += glm::vec2(0.0f,1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) {direction += glm::vec2(0.0f,-1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::D)) {direction += glm::vec2(1.0f,0.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::A)) {direction += glm::vec2(-1.0f,0.0f);}

	if (Util::Input::IsKeyPressed(Util::Keycode::HOME)) {m_Camera.ZoomCamera(0.1f / 10);}
	if (Util::Input::IsKeyPressed(Util::Keycode::END)) {m_Camera.ZoomCamera(-0.1f / 10);}

	if (Util::Input::IsKeyPressed(Util::Keycode::PAGEUP)) {m_Camera.RotateCamera(1.0f / 100);}
	if (Util::Input::IsKeyPressed(Util::Keycode::PAGEDOWN)) {m_Camera.RotateCamera(-1.0f / 100);}

	if (Util::Input::IsKeyUp(Util::Keycode::Y))
	{
		glm::vec2 cursor = Tool::GetMouseCoord();
		LOG_DEBUG("cursor{}", cursor);
		LOG_DEBUG("m_RedShawl {}",m_RedShawl->GetTransform().translation);
		LOG_DEBUG("m_Text {}",m_Text->GetTransform().translation);
		LOG_DEBUG("m_Character {}",m_Character->GetTransform().translation);
		LOG_DEBUG("");
	}

	if (direction.x != 0 || direction.y != 0)
	{
		m_Character->m_Transform.translation += direction * Util::Time::GetDeltaTimeMs() / 5.0f / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		m_Camera.MoveCamera(direction);
	}

	m_Camera.Update();
	m_Root.Update();
}

void TestScene::Exit()
{
	LOG_DEBUG("Test Scene exited");
}

Scene::SceneType TestScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
