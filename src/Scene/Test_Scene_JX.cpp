//
// Created by QuzzS on 2025/3/4.
//

#include "Cursor.hpp"
#include "Scene/Test_Scene_JX.hpp"

#include <filesystem>
#include <iostream>
#include "BulletManager.hpp"
#include "Components/AttackComponent.hpp"
#include "EnumTypes.hpp"
#include "InputManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene_JX::Start()
{
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
	LOG_DEBUG("Entering JX Test Scene");
	m_Wall->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Lobby/Lobby.png"));
	m_Wall->SetZIndex(1);
	m_Wall->m_WorldCoord = glm::vec2(0,16*5);

	m_Enemy->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Sprite/monster小怪/冰原/enemy31礦工/enemy31_0.png"));
	m_Enemy->SetZIndex(10);
	m_Enemy->m_WorldCoord = glm::vec2(16*2,16*1);
	auto MovementComp = m_Enemy->AddComponent<MovementComponent>(ComponentType::MOVEMENT);
	MovementComp->SetMaxSpeed(250.0f);
	auto CollisionComp2 = m_Enemy->AddComponent<CollisionComponent>(ComponentType::COLLISION);
	CollisionComp2->SetCollisionLayer(CollisionLayers_Terrain);
	CollisionComp2->SetCollisionMask(CollisionLayers_Player);
	m_RoomCollisionManager->RegisterNGameObject(m_Enemy);

	m_Player->SetZIndex(12);
	m_Player->m_WorldCoord = {0,16*2}; //騎士初始位置為右兩格，上兩格
	m_RoomCollisionManager->RegisterNGameObject(m_Player);

	//加入m_Root大家庭，才可以被渲染到熒幕上
	m_Root.AddChild(m_Wall);
	m_Root.AddChild(m_Player);
	m_Root.AddChild(m_Enemy);

	//加入了Camera大家庭，Camera移動會被影響，沒加就不會被影響
	// 只對需要跟隨鏡頭的物件（如玩家、地圖）呼叫
	//例如UI，不加入就可以固定在熒幕上
	m_Camera->AddRelativePivotChild(m_Wall);
	m_Camera->AddRelativePivotChild(m_Player);
	m_Camera->AddRelativePivotChild(m_Player->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetCurrentWeapon());
	m_Camera->AddRelativePivotChild(m_Enemy);

	// Camera跟隨player
	m_Camera->SetFollowTarget(m_Player);
	LOG_DEBUG("Starting--->Checked");
}

void TestScene_JX::Input()
{
	//目前還沒開發，專門處理玩家輸入
	//但是要把數據傳入Update需要將data儲存，在Update呼叫
	//需要討論各場景玩家輸入事件會有什麽data需要存
}

void TestScene_JX::Update()
{
	const auto& Bullets = BulletManager::GetInstance().GetBullets();
	for (const auto& bullet : Bullets) {
		m_Root.RemoveChild(bullet);
		m_Camera->RemoveRelativePivotChild(bullet);
	}
	//LOG_DEBUG("Test Scene is running...");
	float deltaTime = Util::Time::GetDeltaTimeMs();
	Cursor::SetWindowOriginWorldCoord(m_Camera->GetCameraWorldCoord().translation); //實時更新Cursor的世界坐標


	// Input：位移量
	inputManager->listenInput();

	//Camera Zoom In=I /Out=K
	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
	{
		glm::vec2 cursor = Cursor::GetCursorWorldCoord(m_Camera->GetCameraWorldCoord().scale.x);
		LOG_DEBUG("Cursor coord:{}", cursor);
	}
	BulletManager::GetInstance().Update();
	const auto& activeBullets = BulletManager::GetInstance().GetBullets();
	for (const auto& bullet : activeBullets) {
		m_Root.AddChild(bullet);
		m_Camera->AddRelativePivotChild(bullet);
	}
	m_Player->Update();
	m_RoomCollisionManager->UpdateCollision();
	m_Camera->Update(); //更新Camera大家庭成員的渲染坐標
	m_Root.Update();
}

void TestScene_JX::Exit()
{
	LOG_DEBUG("JX Test Scene exited");
}

Scene::SceneType TestScene_JX::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		//回傳需要切換的下個場景
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	//沒有要切換就一直回傳Null
	return Scene::SceneType::Null;
}

