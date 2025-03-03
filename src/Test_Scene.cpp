//
// Created by QuzzS on 2025/2/28.
//
#include "Test_Scene.hpp"
#include "Cursor.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene::Start()
{
	LOG_DEBUG("Entering Test Scene");

	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/T.png"));
	//m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(1);

	m_Character->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/pet00icon.png"));
	m_Character->SetZIndex(10);

	m_Enemy->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/knight_0_0.png"));
	m_Enemy->SetZIndex(11);
	m_Enemy->m_WorldCoord = {16*2,16*2};

	m_Weapon->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/weapons_19.png"));
	m_Weapon->SetZIndex(12);
	m_Weapon->m_WorldCoord = m_Enemy->m_WorldCoord;

	// m_Beacon.SetReferenceObjectCoord(std::make_shared<glm::vec2>(Cursor::GetCursorWorldCoord()));

	m_Root.AddChild(m_Background);
	m_Root.AddChild(m_Character);
	m_Root.AddChild(m_Enemy);
	m_Root.AddChild(m_Weapon);

	m_Camera.AddRelativePivotChild(m_Background);
	m_Camera.AddRelativePivotChild(m_Character);
	m_Camera.AddRelativePivotChild(m_Enemy);

	m_Camera.AddRelativePivotChild(m_Weapon);
}

void TestScene::Input()
{
}

void TestScene::Update()
{
	//LOG_DEBUG("Test Scene is running...");
	Cursor::SetWindowOriginWorldCoord(m_Camera.GetCameraWorldCoord().translation);
	m_Weapon->m_WorldCoord = m_Enemy->m_WorldCoord;
	m_Beacon.Update(m_Character->m_WorldCoord,Cursor::GetCursorWorldCoord());

	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_RB))
	{
		LOG_DEBUG("cursor {} {} {}", m_Character->GetWorldCoord(), Cursor::GetCursorWorldCoord(), m_Beacon.GetBeaconWorldCoord());
	}
	m_Camera.Update();

	glm::vec2 direction = {0.0f,0.0f};
	if (Util::Input::IsKeyPressed(Util::Keycode::W)) {direction += glm::vec2(0.0f,1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) {direction += glm::vec2(0.0f,-1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::D)) {direction += glm::vec2(1.0f,0.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::A)) {direction += glm::vec2(-1.0f,0.0f);}

	if (Util::Input::IsKeyPressed(Util::Keycode::I)) {m_Camera.ZoomCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::K)) {m_Camera.ZoomCamera(-1);}

	if (Util::Input::IsKeyPressed(Util::Keycode::O)) {m_Camera.RotateCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::L)) {m_Camera.RotateCamera(-1);}



	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
	{
		glm::vec2 cursor = Cursor::GetCursorWorldCoord();
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
