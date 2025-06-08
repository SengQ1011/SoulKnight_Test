//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Animation.hpp"
#include "ImagePoolManager.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "RoomObject/DestructibleBox.hpp"

#include "RoomObject/WallObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

// 新增的頭文件
#include "Creature/Character.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/ChestComponent.hpp"
#include "EnumTypes.hpp"
#include "Factory/WeaponFactory.hpp"
#include "Scene/SceneManager.hpp"
#include "RandomUtil.hpp"


// class可能是指定類型再用， 目前都是RoomObject
std::shared_ptr<nGameObject> RoomObjectFactory::CreateRoomObject(const std::string &_id, const std::string &_class)
{
	if (!m_ObjectDataFilePath.data())
	{
		LOG_DEBUG("RoomObjectFactory::createRoomObject no set ObjectDataPath");
		return nullptr;
	}
	// 是否是動畫
	bool isAnimated = false;
	nlohmann::json jsonData = m_Loader.lock()->LoadObjectData(_id);
	if (jsonData.contains("isAnimated"))
		isAnimated = jsonData["isAnimated"].get<bool>();

	std::shared_ptr<nGameObject> roomObject;

	// 根據 className 創建特定類型的對象
	if (!_class.empty())
	{
		if (_class == "Wall" || _class == "WallObject")
		{
			roomObject = std::make_shared<WallObject>(_id);
		}
		else if (_class == "DestructibleBox")
		{
			LOG_DEBUG("RoomObjectFactory::createRoomObject destructibleBox");
			roomObject = std::make_shared<DestructibleBox>(_id);
			roomObject->AddComponent<HealthComponent>(ComponentType::HEALTH,1,0,0);
		}
		else
		{
			// 對於其他類型，使用原有邏輯
			if (isAnimated)
			{
				std::vector<std::string> path = jsonData["path"].get<std::vector<std::string>>();
				for (auto &i : path)
					i = RESOURCE_DIR + i;
				std::shared_ptr<Animation> animation = std::make_shared<Animation>(path, true, _class, 0);
				// TODO interval 間隔
				animation->PlayAnimation(true);
				roomObject = animation;
			}
			else
			{
				roomObject = std::make_shared<nGameObject>(_id, _class);
			}
		}
		// roomObject
	}
	else
	{
		// 沒有指定 className 時的默認行為
		if (isAnimated)
		{
			std::vector<std::string> path = jsonData["path"].get<std::vector<std::string>>();
			for (auto &i : path)
				i = RESOURCE_DIR + i;
			std::shared_ptr<Animation> animation = std::make_shared<Animation>(path, true);
			animation->PlayAnimation(true);
			roomObject = animation;
		}
		else
		{
			roomObject = std::make_shared<nGameObject>(_id);
		}
	}

	// 設置Drawable（Animation 已經在構造函數中設置了）
	if (jsonData.contains("path") && !isAnimated)
	{
		roomObject->SetDrawable(
			ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR + jsonData.at("path").get<std::string>()));
	}
	else if (!jsonData.contains("path") && !isAnimated)
	{
		LOG_WARN("RoomObjectFactory::createRoomObject: No path for {}", _id);
	}

	// 設置ZIndexLayer
	if (jsonData.contains("ZIndex"))
	{
		const auto zIndexStr = jsonData.at("ZIndex").get<std::string>();
		roomObject->SetZIndexType(stringToZIndexType(zIndexStr));
	}
	else
	{
		LOG_WARN("RoomObjectFactory::createRoomObject: No ZIndex for {}", _id);
		roomObject->SetZIndexType(ZIndexType::CUSTOM); // 設置默認值
		roomObject->SetZIndex(100.0f);
	}

	// 設置posOffset
	if (jsonData.contains("posOffset"))
	{
		const auto data_posOffset = jsonData.at("posOffset");
		const auto posOffset = glm::vec2(data_posOffset[0].get<float>(), data_posOffset[1].get<float>());
		roomObject->SetPosOffset(posOffset);
	}

	// 設置Components
	if (!jsonData.contains("components"))
		return roomObject; // 沒有就跳過
	for (auto &component : jsonData.at("components"))
	{
		try
		{
			Factory::createComponent(roomObject, component);
		}
		catch (const std::exception &e)
		{
			LOG_ERROR("RoomObjectFactory::createRoomObject: {}", e.what());
		}
	}
	return roomObject;
}

// 專門用於地形創建的方法
std::shared_ptr<nGameObject> RoomObjectFactory::CreateWall(int row, int col, const glm::vec2 &worldPos)
{
	auto wall = std::static_pointer_cast<WallObject>(CreateRoomObject("w604", "Wall"));
	if (wall)
	{
		wall->SetWorldCoord(worldPos);
		wall->SetGridPosition({col, row});
		// 調整牆壁的位置偏移
		auto currentOffset = wall->GetPosOffset();
		wall->SetPosOffset(currentOffset + glm::vec2{0, 1.5f});
	}
	return wall;
}

std::shared_ptr<nGameObject> RoomObjectFactory::CreateFloor(const glm::vec2 &worldPos)
{
	auto floor = CreateRoomObject("f601", "Floor");
	if (floor)
	{
		floor->SetWorldCoord(worldPos);
	}
	return floor;
}

std::shared_ptr<nGameObject> RoomObjectFactory::CreateDoor(const glm::vec2 &worldPos)
{
	auto door = CreateRoomObject("object_door_0", "Door");
	if (door)
	{
		door->SetWorldCoord(worldPos);
	}
	return door;
}

std::shared_ptr<nGameObject> RoomObjectFactory::CreateChest(ChestType type,
															const std::shared_ptr<Character> &player)
{
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	std::shared_ptr<nGameObject> chest;

	if (type == ChestType::REWARD)
	{
		chest = CreateRoomObject("object_rewardChest");
	}
	else if (type == ChestType::WEAPON)
	{
		chest = CreateRoomObject("object_weaponChest");
	}
	if (!chest)
		return nullptr;

	if (const auto interactableComp = chest->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		if (const auto textPrompt = interactableComp->GetPromptObject())
		{
			currentScene->GetPendingObjects().emplace_back(textPrompt);
			textPrompt->SetRegisteredToScene(true);
		}
	}

	if (auto chestComp = chest->GetComponent<ChestComponent>(ComponentType::CHEST))
	{
		std::vector<std::shared_ptr<nGameObject>> objects;
		if (type == ChestType::REWARD)
		{
			int num1 = RandomUtil::RandomFloatInRange(5, 10);
			int num2 = RandomUtil::RandomFloatInRange(5, 10);
			// 生成金幣
			for (int i = 0; i < num1; i++)
			{
				auto coin = CreateRoomObject("object_coin");
				if (!coin)
					LOG_ERROR("Failed to create room object");
				currentScene->GetRoot().lock()->AddChild(coin);
				currentScene->GetCamera().lock()->SafeAddChild(coin);
				coin->SetRegisteredToScene(true);
				coin->SetActive(false);
				coin->SetControlVisible(false);
				coin->SetInitialScale(glm::vec2(0.8f, 0.8f));

				objects.push_back(coin);
			}
			// 生成能量球
			for (int i = 0; i < num2; i++)
			{
				auto energyBall = CreateRoomObject("object_energyBall");
				if (!energyBall)
					LOG_ERROR("Failed to create room object");
				currentScene->GetRoot().lock()->AddChild(energyBall);
				currentScene->GetCamera().lock()->SafeAddChild(energyBall);
				energyBall->SetRegisteredToScene(true);
				energyBall->SetActive(false);
				energyBall->SetControlVisible(false);
				energyBall->SetInitialScale(glm::vec2(0.8f, 0.8f));

				objects.push_back(energyBall);
			}
		}
		else if (type == ChestType::WEAPON)
		{
			std::vector<int> allPlayerWeaponID;
			if (auto attackComp = player->GetComponent<AttackComponent>(ComponentType::ATTACK))
			{
				allPlayerWeaponID = attackComp->GetAllWeaponID();
			}
			auto weapon = WeaponFactory::createRandomWeapon(allPlayerWeaponID);
			if (!weapon)
				LOG_ERROR("Failed to create room object");
			currentScene->GetRoot().lock()->AddChild(weapon);
			currentScene->GetCamera().lock()->SafeAddChild(weapon);
			weapon->SetRegisteredToScene(true);
			weapon->SetControlVisible(false);
			objects.push_back(weapon);
		}

		chestComp->AddDropItems(objects);
	}

	// 因爲可能是在游戲中創建，要手動加入渲染器/manager
	currentScene->GetRoot().lock()->AddChild(chest);
	currentScene->GetCamera().lock()->SafeAddChild(chest);
	chest->SetRegisteredToScene(true);

	return chest;
}
