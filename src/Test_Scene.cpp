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
	m_Tile->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/T.png"));
	m_Tile->SetZIndex(0);

	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/T.png"));
	//m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(1);

	m_Title->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/Title.png"));
	m_Title->SetZIndex(5);
	// m_Title->SetPivot({0,0});
	// m_Title->m_Transform.translation = glm::vec2(-234.0f,221.5f);
	m_Title->m_WorldCoord = glm::vec2(-234,221.5);

	// m_RedShawl->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/RedShawl.png"));
	// m_RedShawl->SetZIndex(4);
	// m_RedShawl->m_Offset = glm::vec2(237,22);

	m_Character->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/pet00icon.png"));
	m_Character->SetZIndex(10);

	m_Text->Init( 20, 2, glm::vec2(-11,300) );
	m_Version->Init( 20, 2, glm::vec2(-451,300) );

	m_Root.AddChild(m_Background);
	// m_Root.AddChild(m_Tile);
	// m_Root.AddChild(m_RedShawl);
	// m_Root.AddChild(m_Title);
	// m_Root.AddChild(m_Version);
	// m_Root.AddChild(m_Text);
	m_Root.AddChild(m_Character);

	m_Camera.AddChild(m_Background);
	// m_Camera.AddChild(m_Tile);
	m_Camera.AddChild(m_RedShawl);
	m_Camera.AddChild(m_Title);
	// m_Camera.AddChild(m_Version);
	// m_Camera.AddChild(m_Text);
	m_Camera.AddChild(m_Character);
}

void TestScene::Input()
{
}

void TestScene::Update()
{
	//LOG_DEBUG("Test Scene is running...");
	m_Camera.Update();

	glm::vec2 direction = {0.0f,0.0f};
	if (Util::Input::IsKeyPressed(Util::Keycode::W)) {direction += glm::vec2(0.0f,1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) {direction += glm::vec2(0.0f,-1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::D)) {direction += glm::vec2(1.0f,0.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::A)) {direction += glm::vec2(-1.0f,0.0f);}

	if (Util::Input::IsKeyPressed(Util::Keycode::HOME)) {m_Camera.ZoomCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::END)) {m_Camera.ZoomCamera(-1);}

	if (Util::Input::IsKeyPressed(Util::Keycode::PAGEUP)) {m_Camera.RotateCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::PAGEDOWN)) {m_Camera.RotateCamera(-1);}

	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
	{
		glm::vec2 cursor = Tool::GetMouseCoord();
		LOG_DEBUG("cursor{}", cursor);
	}

	if (Util::Input::IsKeyUp(Util::Keycode::Y))
	{
		// LOG_DEBUG("m_RedShawl {} ! {}",m_RedShawl->GetTransform(),m_RedShawl->GetPivot());
		// LOG_DEBUG("m_Text {} ! {}",m_Text->GetTransform(),m_Text->GetPivot());
		LOG_DEBUG("m_Background1 {} ! {}",m_Background->GetTransform(),m_Background->GetPivot());
		LOG_DEBUG("m_Background2 {}", m_Background->GetWorldCoord());
		// LOG_DEBUG("m_RedShawl1 {} ! {}",m_RedShawl->GetTransform(),m_RedShawl->GetPivot());
		// LOG_DEBUG("m_RedShawl2 {} ! {}", m_RedShawl->GetOffset(), m_RedShawl->GetWorldCoord());
		LOG_DEBUG("m_Character {} ! {}",m_Character->GetTransform(),m_Character->GetPivot());
		LOG_DEBUG("m_Character {}",m_Character->GetWorldCoord());
		LOG_DEBUG("m_Camera1 {}",m_Camera.GetCameraWorldCoord());
		LOG_DEBUG("");
	}

	if (direction.x != 0 || direction.y != 0)
	{
		m_Character->m_WorldCoord += direction * Util::Time::GetDeltaTimeMs() / 5.0f / std::sqrt(direction.x * direction.x + direction.y * direction.y);
		m_Camera.MoveCamera(direction);
	}

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
