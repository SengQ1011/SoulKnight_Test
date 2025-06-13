//
// Created by QuzzS on 2025/3/4.
//

#include "Scene/Test_Scene_JX.hpp"
#include "Cursor.hpp"


#include <filesystem>
#include <fstream>
#include <iostream>
#include "Attack/AttackManager.hpp"


#include "Components/CollisionComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"


void TestScene_JX::Start()
{
	LOG_DEBUG("Entering JX Test Scene");

	// 確保獲取場景數據（測試場景可能不需要數據）
	if (!m_SceneData)
	{
		Scene::Download();
	}

	// 測試場景不需要數據也能正常運行
	LOG_DEBUG("Scene data status: {}", m_SceneData ? "Available" : "Not available");

	roomHeight = 5 * 35 * 16; // 5個房間，每個房間35格，每格16像素
	m_Player = CharacterFactory::GetInstance().createPlayer(1);
	m_Enemy = CharacterFactory::GetInstance().createEnemy(1);

	// 添加天賦
	// std::vector<Talent> talentDatabase = CreateTalentList();  // 創建天賦資料庫
	// if(auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT)){
	// 	talentComp->AddTalent(talentDatabase[2]);
	// }
	std::ifstream file(JSON_DIR "/LobbyObjectPosition.json");
	if (!file.is_open())
	{
		LOG_DEBUG("Error: Unable to open file: {}", "LobbyObjectPosition");
	}

	nlohmann::json jsonData;
	file >> jsonData;

	m_Camera->SetMapSize(roomHeight);

	// for (const auto &elem: jsonData["roomObject"])
	// {
	// 	m_RoomObject.emplace_back(m_Factory->createRoomObject(elem.at("ID").get<std::string>(),
	// elem.at("Class").get<std::string>())); 	const auto x = elem.at("Position")[0].get<float>(); 	const auto y =
	// elem.at("Position")[1].get<float>(); 	const auto position = glm::vec2(x,y);
	// 	m_RoomObject.back()->SetWorldCoord(position);
	// 	LOG_DEBUG("{}",m_RoomObject.back()->GetWorldCoord());
	// }

	// 墻壁碰撞體
	std::vector<glm::vec2> offset = {glm::vec2(-296.0f, 0.0f),	glm::vec2(-160.0f, 144.0f), glm::vec2(0.0f, 152.0f),
									 glm::vec2(160.0f, 144.0f), glm::vec2(296.0f, 0.0f),	glm::vec2(0.0f, -200.0f)};

	std::vector<glm::vec2> size = {glm::vec2(16.0f, 384.0f), glm::vec2(256.0f, 96.0f), glm::vec2(64.0f, 80.0f),
								   glm::vec2(256.0f, 96.0f), glm::vec2(16.0f, 384.0f), glm::vec2(608.0f, 16.0f)};

	for (int i = 0; i < offset.size(); i++)
	{
		auto wallCollider = std::make_shared<nGameObject>();
		auto collisionComponent = wallCollider->AddComponent<CollisionComponent>(ComponentType::COLLISION);
		collisionComponent->SetOffset(offset[i]);
		collisionComponent->SetSize(size[i]);
		collisionComponent->SetCollisionLayer(CollisionLayers_Terrain);
		m_WallCollider.emplace_back(wallCollider);
	}

	std::for_each(m_WallCollider.begin(), m_WallCollider.end(),
				  [&](std::shared_ptr<nGameObject> obj) { m_RoomCollisionManager->RegisterNGameObject(obj); });
	for (const auto &elem : m_RoomObject)
	{
		if (elem == nullptr)
			continue;
		if (auto collisionComponent = elem->GetComponent<CollisionComponent>(ComponentType::COLLISION))
		{
			m_RoomCollisionManager->RegisterNGameObject(elem);
		}
		m_Camera->AddChild(elem);
		SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(elem);
	}

	// manager setting
	AddManager(ManagerTypes::ATTACK, m_attackManager);
	AddManager(ManagerTypes::INPUT, inputManager);
	AddManager(ManagerTypes::ROOMCOLLISION, m_RoomCollisionManager);
	AddManager(ManagerTypes::TRACKING, m_trackingManager);

	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
	m_trackingManager->AddTerrainObjects(m_RoomObject);
	m_trackingManager->AddTerrainObjects(m_WallCollider);


	m_Enemy->m_WorldCoord = {32, 16 * 2};
	auto collision2 = m_Enemy->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	m_RoomCollisionManager->RegisterNGameObject(m_Enemy);
	m_trackingManager->AddEnemy(m_Enemy);
	m_Root->AddChild(m_Enemy);
	m_Camera->AddChild(m_Enemy);

	m_Player->m_WorldCoord = {0, 16 * 2}; // 騎士初始位置為右兩格，上兩格
	auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	m_RoomCollisionManager->RegisterNGameObject(m_Player);
	m_trackingManager->SetPlayer(m_Player);
	m_Root->AddChild(m_Player);
	m_Camera->AddChild(m_Player);


	// Camera跟隨player
	m_Camera->SetFollowTarget(m_Player);

	LOG_DEBUG("Scene init finish:");
}

void TestScene_JX::Update()
{
	// Input：
	inputManager->Update();
	if (Util::Input::IsKeyDown(Util::Keycode::O))
		m_RoomCollisionManager->ShowColliderBox(); // 按鍵O可以顯示關閉colliderBox

	auto healthComp = m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH);
	auto healthEnemy = m_Enemy->GetComponent<HealthComponent>(ComponentType::HEALTH);
	if (Util::Input::IsKeyPressed(Util::Keycode::Z))
		LOG_DEBUG("Player==>hp: {}, armor: {}, energy: {}", healthComp->GetCurrentHp(), healthComp->GetCurrentArmor(),
				  healthComp->GetCurrentEnergy());
	if (Util::Input::IsKeyPressed(Util::Keycode::X))
		LOG_DEBUG("Enemy==>hp: {}", healthEnemy->GetCurrentHp());

	Cursor::SetWindowOriginWorldCoord(m_Camera->GetCameraWorldCoord().translation); // 實時更新Cursor的世界坐標

	if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_LB))
	{
		glm::vec2 cursor = Cursor::GetCursorWorldCoord(m_Camera->GetCameraWorldCoord().scale.x);
		LOG_DEBUG("Cursor coord:{}", cursor);
	}

	m_attackManager->Update();
	m_trackingManager->Update();

	std::for_each(m_RoomObject.begin(), m_RoomObject.end(), [](std::shared_ptr<nGameObject> obj) { obj->Update(); });
	std::for_each(m_WallCollider.begin(), m_WallCollider.end(),
				  [](std::shared_ptr<nGameObject> obj) { obj->Update(); });
	m_Player->Update();
	m_Enemy->Update();

	m_RoomCollisionManager->Update();
	m_Camera->Update();
	SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->Update();
}

void TestScene_JX::Exit() { LOG_DEBUG("JX Test Scene exited"); }

Scene::SceneType TestScene_JX::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	{
		// 回傳需要切換的下個場景
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::Menu;
	}
	// 沒有要切換就一直回傳Null
	return Scene::SceneType::Null;
}
