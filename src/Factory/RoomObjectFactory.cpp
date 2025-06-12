//
// Created by QuzzS on 2025/3/27.
//

#include "Factory/RoomObjectFactory.hpp"

#include "Animation.hpp"
#include "ImagePoolManager.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "RoomObject/DestructibleObject.hpp"

#include "RoomObject/WallObject.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

// 新增的頭文件
#include "Components/AttackComponent.hpp"
#include "Components/ChestComponent.hpp"
#include "Components/DestructibleEffectComponent.hpp"
#include "Components/DropComponent.hpp"
#include "Components/WalletComponent.hpp"
#include "Creature/Character.hpp"
#include "DestructionEffects/ExplosionEffect.hpp"
#include "DestructionEffects/IceSpikeEffect.hpp"
#include "DestructionEffects/PoisonCloudEffect.hpp"
#include "EnumTypes.hpp"
#include "Factory/WeaponFactory.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"
#include "Shop/ShopTable.hpp"


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
		else if (_class == "DestructibleObject")
		{
			LOG_DEBUG("RoomObjectFactory::createRoomObject destructibleObject");

			// 從 JSON 中讀取圖片路徑陣列
			std::vector<std::string> imagePaths;
			if (jsonData.contains("path"))
			{
				if (jsonData["path"].is_array())
				{
					// 如果 path 是陣列，讀取所有圖片路徑
					auto pathArray = jsonData["path"].get<std::vector<std::string>>();
					for (const auto &path : pathArray)
					{
						imagePaths.push_back(path);
					}
				}
				else if (jsonData["path"].is_string())
				{
					// 如果 path 是字串，只有一張圖片
					imagePaths.push_back(jsonData["path"].get<std::string>());
				}
			}

			// 創建 DestructibleObject，傳遞圖片陣列
			roomObject = std::make_shared<DestructibleObject>(_id, imagePaths);
			roomObject->AddComponent<HealthComponent>(ComponentType::HEALTH, 1, 0, 0);

			if (_id == "object_boxRed")
			{
				auto explosionEffect = std::make_unique<ExplosionEffect>(40.0f, 8);
				roomObject->AddComponent<DestructibleEffectComponent>(ComponentType::DESTRUCTIBLE_EFFECT,
																	  std::move(explosionEffect));
			}
			else if (_id == "object_boxGreen")
			{
				// 綠色箱子：毒圈效果
				auto poisonEffect = std::make_unique<PoisonCloudEffect>(60.0f, 3, 8.0f);
				roomObject->AddComponent<DestructibleEffectComponent>(ComponentType::DESTRUCTIBLE_EFFECT,
																	  std::move(poisonEffect));
			}
			else if (_id == "object_boxBlue")
			{
				// 藍色箱子：冰刺效果
				auto iceSpikeEffect = std::make_unique<IceSpikeEffect>(8, 5, 250.0f);
				roomObject->AddComponent<DestructibleEffectComponent>(ComponentType::DESTRUCTIBLE_EFFECT,
																	  std::move(iceSpikeEffect));
			}
			else if (_id.find("object_boxCommon") != std::string::npos)
			{
				const float hasDropItem = RandomUtil::RandomFloatInRange(0.0f, 1.0f);
				if (hasDropItem < 0.2f)
				{
					int energyCount = RandomUtil::RandomIntInRange(1, 5);
					auto energyBalls = CreateDropItems("object_energyBall", energyCount, 0.8f);

					// 添加 DropComponent 來處理掉落
					if (!energyBalls.empty())
					{
						auto dropComponent = roomObject->AddComponent<DropComponent>(
							ComponentType::DROP, DropComponent::ScatterMode::SLIGHT);
						dropComponent->AddDropItems(energyBalls);
					}
				}
			}
		}
		else if (_class == "ShopTable")
		{
			roomObject = std::make_shared<ShopTable>(_id, _class);
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
			std::shared_ptr<Animation> animation = std::make_shared<Animation>(path, true, "Animation", 0);
			animation->PlayAnimation(true);
			roomObject = animation;
		}
		else
		{
			roomObject = std::make_shared<nGameObject>(_id);
		}
	}

	// 設置Drawable（Animation 和 DestructibleObject 已經在構造函數中設置了）
	if (jsonData.contains("path") && !isAnimated && _class != "DestructibleObject")
	{
		// 對於非 DestructibleObject 的物件，設置單一圖片
		if (jsonData["path"].is_string())
		{
			roomObject->SetDrawable(
				ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR + jsonData.at("path").get<std::string>()));
		}
		else if (jsonData["path"].is_array())
		{
			// 如果是陣列，隨機選擇一張圖片
			auto pathArray = jsonData["path"].get<std::vector<std::string>>();
			if (!pathArray.empty())
			{
				// 使用 RandomUtil 隨機選擇一個索引
				int randomIndex = RandomUtil::RandomIntInRange(0, static_cast<int>(pathArray.size()) - 1);
				roomObject->SetDrawable(
					ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR + pathArray[randomIndex]));
			}
		}
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
	auto wall = std::static_pointer_cast<WallObject>(CreateRoomObject("w600", "Wall"));
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
	auto floor = CreateRoomObject("f600", "Floor");
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

std::shared_ptr<nGameObject> RoomObjectFactory::CreateChest(ChestType type, const std::shared_ptr<Character> &player)
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

	// 創建掉落物品並添加到 DropComponent
	std::vector<std::shared_ptr<nGameObject>> objects;
	if (type == ChestType::REWARD)
	{
		int coinCount = RandomUtil::RandomFloatInRange(5, 10);
		int energyCount = RandomUtil::RandomFloatInRange(5, 10);

		// 生成金幣
		auto coins = CreateDropItems("object_coin", coinCount, 0.8f);
		objects.insert(objects.end(), coins.begin(), coins.end());

		// 生成能量球
		auto energyBalls = CreateDropItems("object_energyBall", energyCount, 0.8f);
		objects.insert(objects.end(), energyBalls.begin(), energyBalls.end());
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

	// 添加 DropComponent 來處理掉落
	if (!objects.empty())
	{
		// 根據寶箱類型選擇散佈模式
		DropComponent::ScatterMode scatterMode =
			(type == ChestType::WEAPON) ? DropComponent::ScatterMode::FIXED : DropComponent::ScatterMode::RANDOM;

		auto dropComponent = chest->AddComponent<DropComponent>(ComponentType::DROP, scatterMode);
		dropComponent->AddDropItems(objects);
	}

	// 因爲可能是在游戲中創建，要手動加入渲染器/manager
	currentScene->GetRoot().lock()->AddChild(chest);
	currentScene->GetCamera().lock()->SafeAddChild(chest);
	chest->SetRegisteredToScene(true);

	return chest;
}

std::shared_ptr<DestructibleObject> RoomObjectFactory::CreateMine(MineType type)
{
	std::shared_ptr<DestructibleObject> mine;
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	switch (type)
	{
	case MineType::ENERGY:
		{
			// 創建能量礦物
			mine = std::static_pointer_cast<DestructibleObject>(
				CreateRoomObject("object_energyMine_0", "DestructibleObject"));

			if (mine)
			{
				int energyCount = RandomUtil::RandomIntInRange(10, 15);
				auto energyBalls = CreateDropItems("object_energyBall", energyCount, 0.8f);

				// 添加 DropComponent 來處理掉落
				if (!energyBalls.empty())
				{
					auto dropComponent =
						mine->AddComponent<DropComponent>(ComponentType::DROP, DropComponent::ScatterMode::RANDOM);
					dropComponent->AddDropItems(energyBalls);
				}

				if (auto healthComponent = mine->GetComponent<HealthComponent>(ComponentType::HEALTH))
				{
					healthComponent->SetMaxHp(10);
					healthComponent->SetCurrentHp(10);
				}
			}
			break;
		}
	case MineType::GOLD:
		{
			// 創建金幣礦物
			mine = std::static_pointer_cast<DestructibleObject>(
				CreateRoomObject("object_goldMine_0", "DestructibleObject"));

			if (mine)
			{
				int coinCount = RandomUtil::RandomIntInRange(5, 15);
				auto coins = CreateDropItems("object_coin", coinCount, 0.8f);

				// 添加 DropComponent 來處理掉落
				if (!coins.empty())
				{
					auto dropComponent =
						mine->AddComponent<DropComponent>(ComponentType::DROP, DropComponent::ScatterMode::SLIGHT);
					dropComponent->AddDropItems(coins);
				}

				if (auto healthComponent = mine->GetComponent<HealthComponent>(ComponentType::HEALTH))
				{
					healthComponent->SetMaxHp(10);
					healthComponent->SetCurrentHp(10);
				}
			}
			break;
		}
	default:
		LOG_ERROR("Failed to RoomObjectFactory::CreateMine: Unknown mine type");
		return nullptr;
	}

	// 如果是在遊戲中創建，要手動加入渲染器/manager
	if (mine && currentScene)
	{
		currentScene->GetRoot().lock()->AddChild(mine);
		currentScene->GetCamera().lock()->SafeAddChild(mine);
		mine->SetRegisteredToScene(true);
	}

	return mine;
}

std::shared_ptr<nGameObject> RoomObjectFactory::CreateShopTable(ShopItemType itemType, int price,
																const std::shared_ptr<Character> &player,
																PotionSize potionSize)
{
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	// 使用基本的商品桌子對象
	auto shopTableBase = CreateRoomObject("object_shop_itemTable", "ShopTable");
	if (!shopTableBase)
		return nullptr;

	// 轉換為 ShopTable 類型並初始化
	auto shopTable = std::dynamic_pointer_cast<ShopTable>(shopTableBase);
	if (!shopTable)
		return nullptr;

	std::string tableName = "ShopTable_" + std::to_string(static_cast<int>(itemType));
	shopTable->Initialize(tableName, itemType, price, potionSize);
	if (const auto interactableComp = shopTable->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		std::string priceText = std::to_string(price) + " 金幣";
		interactableComp->SetPromptText(priceText);
	}

	// 創建商品並設置在桌子上方
	std::shared_ptr<nGameObject> currentItem = nullptr;
	glm::vec2 itemOffset = glm::vec2(0, -20); // 商品在桌子上方20像素

	switch (itemType)
	{
	case ShopItemType::HEALTH_POTION:
		{
			currentItem = CreatePotion(PotionType::HEALTH, potionSize);
			break;
		}
	case ShopItemType::ENERGY_POTION:
		{
			currentItem = CreatePotion(PotionType::ENERGY, potionSize);
			break;
		}
	case ShopItemType::WEAPON:
		{
			// 獲取玩家現有武器ID列表，避免重複
			std::vector<int> allPlayerWeaponID;
			if (auto attackComp = player->GetComponent<AttackComponent>(ComponentType::ATTACK))
			{
				allPlayerWeaponID = attackComp->GetAllWeaponID();
			}

			// 創建隨機武器
			currentItem = WeaponFactory::createRandomWeapon(allPlayerWeaponID);
			if (!currentItem)
			{
				LOG_ERROR("Failed to create random weapon for shop");
				return shopTable;
			}
			break;
		}
	}

	// 設置商品位置和狀態
	if (currentItem && currentScene)
	{
		// 設置當前商品引用，桌子會在 Update 中更新商品位置
		shopTable->SetCurrentItem(currentItem);
		currentItem->SetWorldCoord(shopTable->GetWorldCoord() + itemOffset);
		currentItem->SetZIndexType(OBJECTHIGH);

		// 註冊商品到場景
		if (itemType == ShopItemType::WEAPON)
		{
			// currentScene->GetRoot().lock()->AddChild(currentItem);
			// currentScene->GetCamera().lock()->SafeAddChild(currentItem);
			currentScene->GetPendingObjects().emplace_back(currentItem);
			currentItem->SetRegisteredToScene(true);
			currentItem->SetControlVisible(true);
		}
		else
		{
			currentScene->GetPendingObjects().emplace_back(currentItem);
			currentItem->SetRegisteredToScene(true);
		}

		// 將商品的 InteractableComponent 設為不活躍（購買前不能直接拾取）
		if (auto itemInteractable = currentItem->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
		{
			itemInteractable->SetComponentActive(false);
		}
	}

	return shopTable;
}

std::shared_ptr<nGameObject> RoomObjectFactory::CreatePotion(PotionType potionType, PotionSize potionSize)
{
	// 根據藥水類型和大小決定 JSON ID
	std::string jsonId;

	switch (potionType)
	{
	case PotionType::HEALTH:
		jsonId = (potionSize == PotionSize::SMALL) ? "object_smallHealthPotion" : "object_bigHealthPotion";
		break;

	case PotionType::ENERGY:
		jsonId = (potionSize == PotionSize::SMALL) ? "object_smallEnergyPotion" : "object_bigEnergyPotion";
		break;

	default:
		LOG_ERROR("Unknown potion type");
		return nullptr;
	}

	// 使用 JSON 配置創建藥水（會自動創建 InteractableComponent）
	std::shared_ptr<nGameObject> potion = CreateRoomObject(jsonId);

	if (!potion)
	{
		LOG_ERROR("Failed to create potion using JSON config: {}", jsonId);
		return nullptr;
	}

	return potion;
}

std::vector<std::shared_ptr<nGameObject>> RoomObjectFactory::CreateDropItems(const std::string &itemType, int quantity,
																			 float scale)
{
	std::vector<std::shared_ptr<nGameObject>> items;
	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();

	if (!currentScene)
	{
		LOG_ERROR("No current scene available for creating drop items");
		return items;
	}

	for (int i = 0; i < quantity; i++)
	{
		auto item = CreateRoomObject(itemType);
		if (!item)
		{
			LOG_ERROR("Failed to create drop item: {}", itemType);
			continue;
		}

		// 設置物品屬性
		currentScene->GetRoot().lock()->AddChild(item);
		currentScene->GetCamera().lock()->SafeAddChild(item);
		item->SetRegisteredToScene(true);
		item->SetActive(false);
		item->SetControlVisible(false);
		item->SetInitialScale(glm::vec2(scale, scale));

		items.push_back(item);
	}

	return items;
}
