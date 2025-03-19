//
// Created by QuzzS on 2025/3/4.
//
#include "Scene/Test_Scene_KC.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Cursor.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene_KC::Start()
{
	LOG_DEBUG("Entering KC Test Scene");

	m_Floor->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/LobbyFloor.png"));
	//m_Floor->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/MainMenu/MainMenuBackground.png"));
	m_Floor->SetZIndex(2);

	m_Wall->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/Lobby.png"));
	m_Wall->SetZIndex(1);
	m_Wall->m_WorldCoord = glm::vec2(0, 16 * 5);

	m_Character->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/knight_0_0.png"));
	m_MoveComp = m_Character->AddComponent<MovementComponent>();
	auto CollisionComp = m_Character->AddComponent<CollisionComponent>();
	CollisionComp->SetCollisionLayer(CollisionLayers_Player);
	CollisionComp->SetCollisionMask(CollisionLayers_Terrain);
	m_MoveComp->SetMaxSpeed(250.0f);
	m_Character->SetZIndex(10);
	m_RoomCollisionManager->RegisterNGameObject(m_Character);

	m_Enemy->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/pet00icon.png"));
	auto MovementComp = m_Enemy->AddComponent<MovementComponent>();
	MovementComp->SetMaxSpeed(250.0f);
	auto CollisionComp2 = m_Enemy->AddComponent<CollisionComponent>();
	CollisionComp2->SetCollisionLayer(CollisionLayers_Terrain);
	CollisionComp2->SetCollisionMask(CollisionLayers_None);
	m_Enemy->SetZIndex(11);
	m_Enemy->m_WorldCoord = {16*2,16*2};
	m_RoomCollisionManager->RegisterNGameObject(m_Enemy);

	m_Weapon->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/weapons_19.png"));
	auto FollowerComp = m_Weapon->AddComponent<FollowerComponent>();
	FollowerComp->SetFollower(m_Character);
	FollowerComp->IsTargetMouse(true);
	FollowerComp->SetHandOffset(glm::vec2(m_Character->GetImageSize().x/7.0f,-m_Character->GetImageSize().x/4.0f));
	FollowerComp->SetHoldingPosition(glm::vec2(m_Weapon->GetImageSize().x/2.0f,0));
	m_Weapon->SetZIndex(12);

	// m_Beacon.SetReferenceObjectCoord(std::make_shared<glm::vec2>(Cursor::GetCursorWorldCoord()));

	// m_Root.AddChild(m_Floor);
	m_Root.AddChild(m_Wall);
	m_Root.AddChild(m_Character);
	m_Root.AddChild(m_Enemy);
	m_Root.AddChild(m_Weapon);

	m_Camera.AddRelativePivotChild(m_Floor);
	m_Camera.AddRelativePivotChild(m_Wall);
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
	// m_Beacon.Update(m_Character->m_WorldCoord,Cursor::GetCursorWorldCoord(m_Camera.GetCameraWorldCoord().scale.x));

	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_RB))
	{
		LOG_DEBUG("cursor {} {} {}", m_Character->GetWorldCoord(), Cursor::GetCursorWorldCoord(m_Camera.GetCameraWorldCoord().scale.x), m_Beacon.GetBeaconWorldCoord());
	}
	m_Camera.Update();

	glm::vec2 direction = {0.0f,0.0f};
	if (Util::Input::IsKeyPressed(Util::Keycode::W)) {direction += glm::vec2(0.0f,1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) {direction += glm::vec2(0.0f,-1.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::D)) {direction += glm::vec2(1.0f,0.0f);}
	if (Util::Input::IsKeyPressed(Util::Keycode::A)) {direction += glm::vec2(-1.0f,0.0f);}

	if (glm::length(direction) > 0.01f)
	{
		direction = glm::normalize(direction);
		LOG_DEBUG("Direction: ({}, {})", direction.x, direction.y);
		m_Character->GetComponent<MovementComponent>()->SetAcceleration(direction);
	}
	m_Character->Update();
	m_Weapon->Update();

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
		LOG_DEBUG("m_Floor1 {} ! {}",m_Floor->GetTransform(),m_Floor->GetPivot());
		LOG_DEBUG("m_Floor2 {}", m_Floor->GetWorldCoord());
		LOG_DEBUG("m_Character {} ! {}",m_Character->GetTransform(),m_Character->GetPivot());
		LOG_DEBUG("m_Character {}",m_Character->GetWorldCoord());
		LOG_DEBUG("m_Camera1 {}",m_Camera.GetCameraWorldCoord());
		LOG_DEBUG("");
	}

	m_Camera.CameraFollowWith( m_Character->m_WorldCoord);

	m_RoomCollisionManager->UpdateCollision();
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
