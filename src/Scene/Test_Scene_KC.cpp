//
// Created by QuzzS on 2025/3/4.
//
#include "Scene/Test_Scene_KC.hpp"
#include "Cursor.hpp"
#include "Override/reverseImage.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene_KC::Start()
{
	LOG_DEBUG("Entering KC Test Scene");

	m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/T.png"));
	//m_Background->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Background->SetZIndex(1);

	m_Character->SetHorizontalMirrorDrawable(
		std::make_shared<Util::Image>(RESOURCE_DIR"/pet00icon.png"),
		std::make_shared<Util::reverseImage>(RESOURCE_DIR"/pet00icon.png"));
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

void TestScene_KC::Input()
{
}

void TestScene_KC::Update()
{
	//LOG_DEBUG("Test Scene is running...")
	Cursor::SetWindowOriginWorldCoord(m_Camera.GetCameraWorldCoord().translation);
	m_Weapon->m_WorldCoord = m_Enemy->m_WorldCoord;
	m_Beacon.Update(m_Character->m_WorldCoord,Cursor::GetCursorWorldCoord(m_Camera.GetCameraWorldCoord().scale.x));

	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_RB))
	{
		LOG_DEBUG("cursor {} {} {}", m_Character->GetWorldCoord(), Cursor::GetCursorWorldCoord(m_Camera.GetCameraWorldCoord().scale.x), m_Beacon.GetBeaconWorldCoord());
	}
	m_Camera.Update();

	glm::vec2 speed = {0.0f,0.0f};
	if (Util::Input::IsKeyPressed(Util::Keycode::W)) {speed += glm::vec2(0.0f,1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) {speed += glm::vec2(0.0f,-1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::D))
	{
		speed += glm::vec2(1.0f,0.0f);
		if(m_Character->GetImageDirection() == nGameObject::GameObjectDirection::Reverse)
		{
			m_Character->ChangeDrawableDirection();
		}
	}
	if (Util::Input::IsKeyPressed(Util::Keycode::A))
	{
		speed += glm::vec2(-1.0f,0.0f);
		if(m_Character->GetImageDirection() == nGameObject::GameObjectDirection::Forward)
		{
			m_Character->ChangeDrawableDirection();
		}
	}

	if (Util::Input::IsKeyPressed(Util::Keycode::I)) {m_Camera.ZoomCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::K)) {m_Camera.ZoomCamera(-1);}

	if (Util::Input::IsKeyPressed(Util::Keycode::O)) {m_Camera.RotateCamera(1);}
	if (Util::Input::IsKeyPressed(Util::Keycode::L)) {m_Camera.RotateCamera(-1);}


	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
	{
		glm::vec2 cursor = Cursor::GetCursorWorldCoord(m_Camera.GetCameraWorldCoord().scale.x);
		LOG_DEBUG("cursor{}", cursor);
	}

	if (Util::Input::IsKeyUp(Util::Keycode::Y))
	{
		LOG_DEBUG("m_Background1 {} ! {}",m_Background->GetTransform(),m_Background->GetPivot());
		LOG_DEBUG("m_Background2 {}", m_Background->GetWorldCoord());
		LOG_DEBUG("m_Character {} ! {}",m_Character->GetTransform(),m_Character->GetPivot());
		LOG_DEBUG("m_Character {}",m_Character->GetWorldCoord());
		LOG_DEBUG("m_Camera1 {}",m_Camera.GetCameraWorldCoord());
		LOG_DEBUG("");
	}

	//TODO:F
	if (speed != glm::vec2(0.0f)) //檢查非零向量 才能進行向量標準化 length(speed) != 0模長非零，因爲normalize = speed / length(speed)
	{
		const float ratio = 0.2f;
		const glm::vec2 deltaDisplacement = normalize(speed) * ratio * Util::Time::GetDeltaTimeMs(); //normalize為防止斜向走速度是根號2
		m_Character->m_WorldCoord += deltaDisplacement;
		m_Camera.MoveCamera(deltaDisplacement);
	}

	m_Root.Update();
}

void TestScene_KC::Exit()
{
	LOG_DEBUG("KC Test Scene exited");
}

Scene::SceneType TestScene_KC::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	return Scene::SceneType::Null;
}
