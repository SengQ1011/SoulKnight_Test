//
// Created by QuzzS on 2025/3/4.
//

#include "Cursor.hpp"
#include "Scene/Test_Scene_JX.hpp"

#include <filesystem>
#include <iostream>
#include "BulletManager.hpp"

#include "EnumTypes.hpp"
#include "InputManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"
#include "Scene/SceneManager.hpp"
#include "Components/AttackComponent.hpp"


void TestScene_JX::Start()
{
	LOG_DEBUG("Entering JX Test Scene");
	m_Player = factory.createPlayer(1);
	m_Enemy = std::make_shared<nGameObject>();

	AddManager("BulletManager",bulletManager);
	AddManager("InputManager",inputManager);
	AddManager("RoomCollisionManager",m_RoomCollisionManager);

	std::ifstream file(JSON_DIR"/LobbyObjectPosition.json");
	if (!file.is_open()) {
		LOG_DEBUG("Error: Unable to open file: {}","LobbyObjectPosition");
	}

	nlohmann::json jsonData;
	file >> jsonData;

	roomHeight = jsonData.at("room_height").get<float>() * jsonData.at("tile_height").get<float>();
	m_Camera->SetMapSize(roomHeight);

	for (const auto &elem: jsonData["roomObject"])
	{
		m_RoomObject.emplace_back(m_Factory->createRoomObject(elem.at("ID").get<std::string>(), elem.at("Class").get<std::string>()));
		const auto x = elem.at("Position")[0].get<float>();
		const auto y = elem.at("Position")[1].get<float>();
		const auto position = glm::vec2(x,y);
		m_RoomObject.back()->SetWorldCoord(position);
		LOG_DEBUG("{}",m_RoomObject.back()->GetWorldCoord());
	}

	// 墻壁碰撞體
	std::vector<glm::vec2> offset = {glm::vec2(-304.0f,-192.0f), glm::vec2(-288.0f,96.0f), glm::vec2(-32.0f,112.0f),
									 glm::vec2(32.0f,96.0f), glm::vec2(288.0f,-192.0f), glm::vec2(-304.0f,-208.0f)};

	std::vector<glm::vec2> size = {glm::vec2(16.0f,384.0f), glm::vec2(256.0f,96.0f), glm::vec2(64.0f,80.0f),
								   glm::vec2(256.0f,96.0f), glm::vec2(16.0f,384.0f), glm::vec2(608.0f,16.0f)};

	for (int i = 0; i < offset.size(); i++)
	{
		auto wallCollider = std::make_shared<nGameObject>();
		auto collisionComponent = wallCollider->AddComponent<CollisionComponent>(ComponentType::COLLISION);
		collisionComponent->SetOffset(offset[i]);
		collisionComponent->SetSize(size[i]);
		collisionComponent->SetCollisionLayer(CollisionLayers_Terrain);
		SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(collisionComponent->GetBlackBox());
		m_Camera->AddRelativePivotChild(collisionComponent->GetBlackBox());
		m_WallCollider.emplace_back(wallCollider);
	}

	std::for_each(m_WallCollider.begin(), m_WallCollider.end(), [&](std::shared_ptr<nGameObject> obj){m_RoomCollisionManager->RegisterNGameObject(obj);});
	for (const auto &elem: m_RoomObject)
	{
		if (elem == nullptr) continue;
		if (elem->GetComponent<CollisionComponent>(ComponentType::COLLISION) != nullptr)
		{
			m_RoomCollisionManager->RegisterNGameObject(elem);
			auto collisionComponent = elem->GetComponent<CollisionComponent>(ComponentType::COLLISION);
			SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(collisionComponent->GetBlackBox());
			m_Camera->AddRelativePivotChild(collisionComponent->GetBlackBox());
		}
		m_Camera->AddRelativePivotChild(elem);
		SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(elem);
	}

	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);

	m_Enemy->SetZIndex(10);
	m_Enemy->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/Sprite/monster小怪/冰原/enemy31礦工/enemy31_0.png"));
	m_Enemy->m_WorldCoord = glm::vec2(16*2,16*1);
	auto MovementComp = m_Enemy->AddComponent<MovementComponent>(ComponentType::MOVEMENT, 1.0f);
	MovementComp->SetMaxSpeed(250.0f);
	auto CollisionComp2 = m_Enemy->AddComponent<CollisionComponent>(ComponentType::COLLISION);
	CollisionComp2->SetCollisionLayer(CollisionLayers_Terrain);
	CollisionComp2->SetSize(glm::vec2(16.0f));
	CollisionComp2->SetCollisionLayer(CollisionLayers_Enemy);
	CollisionComp2->SetCollisionMask(CollisionLayers_Player);
	m_RoomCollisionManager->RegisterNGameObject(m_Enemy);
	m_Root->AddChild(m_Enemy);

	m_Player->m_WorldCoord = {0,16*2}; //騎士初始位置為右兩格，上兩格
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(collision->GetBlackBox());
	m_RoomCollisionManager->RegisterNGameObject(m_Player);

	m_Root->AddChild(m_Player);

	//加入了Camera大家庭，Camera移動會被影響，沒加就不會被影響
	// 只對需要跟隨鏡頭的物件（如玩家、地圖）呼叫
	//例如UI，不加入就可以固定在熒幕上
	m_Camera->AddRelativePivotChild(m_Player);
	m_Camera->AddRelativePivotChild(m_Enemy);


	// Camera跟隨player
	m_Camera->SetFollowTarget(m_Player);

	LOG_DEBUG("Scene init finish:");
}

void TestScene_JX::Update()
{
	// Input：位移量
	inputManager->listenInput();

	float deltaTime = Util::Time::GetDeltaTimeMs();
	Cursor::SetWindowOriginWorldCoord(m_Camera->GetCameraWorldCoord().translation); //實時更新Cursor的世界坐標

	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB)) {
		glm::vec2 cursor = Cursor::GetCursorWorldCoord(m_Camera->GetCameraWorldCoord().scale.x);
		LOG_DEBUG("Cursor coord:{}", cursor);
	}

	bulletManager->Update();

	std::for_each(m_RoomObject.begin(), m_RoomObject.end(), [](std::shared_ptr<nGameObject> obj){obj->Update();});
	std::for_each(m_WallCollider.begin(), m_WallCollider.end(), [](std::shared_ptr<nGameObject> obj){obj->Update();});
	m_Player->Update();
	m_RoomCollisionManager->UpdateCollision();
	m_Camera->Update();
	SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->Update();
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

