//
// Created by QuzzS on 2025/3/4.
//
#include "Scene/Test_Scene_KC.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"

#include "Cursor.hpp"
#include "Scene/SceneManager.hpp"
#include "pch.hpp"

#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


void TestScene_KC::Start()
{
	LOG_DEBUG("Entering KC Test Scene");
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
	std::vector<glm::vec2> offset = {glm::vec2(-296.0f,0.0f), glm::vec2(-160.0f,144.0f), glm::vec2(0.0f,152.0f),
									 glm::vec2(160.0f,144.0f), glm::vec2(296.0f,0.0f), glm::vec2(0.0f,-200.0f)};

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

	m_Player->SetDrawable(std::make_shared<Util::Image>(RESOURCE_DIR"/knight_0_0.png"));
	m_Player->SetWorldCoord(glm::vec2(32));
	auto collisionComponent = m_Player->AddComponent<CollisionComponent>(ComponentType::COLLISION);
	collisionComponent->SetCollisionLayer(CollisionLayers_Player);
	collisionComponent->SetCollisionMask(CollisionLayers_Terrain);
	collisionComponent->SetSize(glm::vec2(16,16));
	collisionComponent->SetOffset(glm::vec2(6,-8));
	SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(collisionComponent->GetBlackBox());
	m_Camera->AddRelativePivotChild(collisionComponent->GetBlackBox());
	auto movementComponent = m_Player->AddComponent<MovementComponent>(ComponentType::MOVEMENT, 1.0f);
	movementComponent->SetMaxSpeed(100);
	movementComponent->SetSpeedRatio(0.9);
	m_RoomCollisionManager->RegisterNGameObject(m_Player);
	m_Camera->AddRelativePivotChild(m_Player);
	SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->AddChild(m_Player);
}

void TestScene_KC::Update()
{
	// movement移動
	float deltaTime = Util::Time::GetDeltaTimeMs();
	glm::vec2 movement = glm::vec2(0, 0);
	if (Util::Input::IsKeyPressed(Util::Keycode::W)) movement.y += 1;
	if (Util::Input::IsKeyPressed(Util::Keycode::S)) movement.y -= 1;
	if (Util::Input::IsKeyPressed(Util::Keycode::A)) movement.x -= 1;
	if (Util::Input::IsKeyPressed(Util::Keycode::D)) movement.x += 1;
	if (Util::Input::IsKeyPressed(Util::Keycode::I)) m_Camera->ZoomCamera(1);
	if (Util::Input::IsKeyPressed(Util::Keycode::K)) m_Camera->ZoomCamera(-1);

	if (Util::Input::IsKeyUp(Util::Keycode::O)) m_RoomCollisionManager->ShowColliderBox();
	if (movement.x != 0.0f || movement.y != 0.0f)
	{
		const glm::vec2 deltaDisplacement = normalize(movement) * 10.0f * deltaTime; //normalize為防止斜向走速度是根號2
		auto movementComponent = m_Player->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
		if ((movement.x < 0 && m_Player->m_Transform.scale.x > 0) ||
			(movement.x > 0 && m_Player->m_Transform.scale.x < 0))
		{
			m_Player->m_Transform.scale.x *= -1.0f;
		}
		m_Player->GetComponent<MovementComponent>(ComponentType::MOVEMENT)->SetDesiredDirection(deltaDisplacement);
	}
	std::for_each(m_RoomObject.begin(), m_RoomObject.end(), [](std::shared_ptr<nGameObject> obj){obj->Update();});
	std::for_each(m_WallCollider.begin(), m_WallCollider.end(), [](std::shared_ptr<nGameObject> obj){obj->Update();});
	m_Player->Update();
	m_Camera->SetFollowTarget(m_Player);
	m_RoomCollisionManager->UpdateCollision();
	m_Camera->Update();
	SceneManager::GetInstance().GetCurrentScene().lock()->GetRoot().lock()->Update();
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
