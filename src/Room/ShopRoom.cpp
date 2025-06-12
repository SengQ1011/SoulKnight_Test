//
// Created by AI Assistant on 2025/4/29.
//

#include "Room/ShopRoom.hpp"
#include "Creature/Character.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Shop/ShopTable.hpp"
#include "Util/Input.hpp"
#include "Util/Logger.hpp"
#include "json.hpp"


void ShopRoom::Start(const std::shared_ptr<Character> &player)
{
	DungeonRoom::Start(player);

	// 初始化商店
	InitializeShop();

	LOG_DEBUG("ShopRoom started at grid position ({}, {})", m_MapGridPos.x, m_MapGridPos.y);
}

void ShopRoom::Update()
{
	DungeonRoom::Update();
	if (Util::Input::IsKeyDown(Util::Keycode::R))
	{
		LOG_DEBUG("ShopTable reflesh");
		RefreshAllItems();
	}
}

void ShopRoom::LoadFromJSON()
{
	// 使用專門的商店佈局檔案
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadShopRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void ShopRoom::TryActivateByPlayer()
{
	// 商店房間被玩家進入時立即設為已探索
	SetState(RoomState::EXPLORED);
}

void ShopRoom::OnStateChanged()
{
	// 當房間狀態改變時的處理
	switch (GetState())
	{
	case RoomState::EXPLORED:
		LOG_DEBUG("ShopRoom explored - shop is open for business");
		break;
	default:
		break;
	}
}

void ShopRoom::InitializeShop()
{
	if (m_ShopInitialized)
		return;

	// 創建商品桌子
	CreateShopTables();

	// 之後會添加商人NPC
	// CreateMerchantNPC();

	m_ShopInitialized = true;
	LOG_DEBUG("Shop initialized successfully");
}

void ShopRoom::CreateShopTables()
{
	// 根據佈局檔案中的商品桌子位置創建商品桌子
	// 這裡暫時先用硬編碼的位置，之後可以從JSON讀取

	const glm::vec2 roomCenter = m_RoomSpaceInfo.m_WorldCoord;

	// 四個商品桌子的位置（對應JSON檔案中的位置）
	std::vector<glm::vec2> tablePositions = {
		roomCenter + glm::vec2(-93.0f, -49.0f), // 左下
		roomCenter + glm::vec2(-31.0f, -49.0f), // 左上
		roomCenter + glm::vec2(31.0f, -49.0f), // 右上
		roomCenter + glm::vec2(93.0f, -49.0f) // 右下
	};

	// 創建商品桌子
	for (size_t i = 0; i < tablePositions.size(); ++i)
	{
		ShopItemType itemType;
		int price;

		// 根據位置決定商品類型和價格
		switch (i)
		{
		case 0: // 生命藥水 - 隨機大小
			{
				itemType = ShopItemType::HEALTH_POTION;
				// 50% 機率生成大藥水，50% 機率生成小藥水
				bool isBigPotion = (rand() % 2 == 0);
				price = isBigPotion ? 20 : 10; // 大藥水價格是小藥水的2倍
				break;
			}
		case 1: // 能量藥水 - 隨機大小
			{
				itemType = ShopItemType::ENERGY_POTION;
				// 50% 機率生成大藥水，50% 機率生成小藥水
				bool isBigPotion = (rand() % 2 == 0);
				price = isBigPotion ? 16 : 8; // 大藥水價格是小藥水的2倍
				break;
			}
		case 2: // 武器1
		case 3: // 武器2
			itemType = ShopItemType::WEAPON;
			price = 50;
			break;
		}

		// 使用 RoomObjectFactory 創建商品桌子
		PotionSize potionSize = PotionSize::SMALL; // 默認值，對武器無效

		// 對藥水類型，根據價格判斷大小
		if (itemType == ShopItemType::HEALTH_POTION || itemType == ShopItemType::ENERGY_POTION)
		{
			// 價格高的是大藥水，價格低的是小藥水
			potionSize = (price > 10) ? PotionSize::BIG : PotionSize::SMALL;
		}

		auto shopTable = m_Factory.lock()->CreateShopTable(itemType, price, m_Player.lock(), potionSize);
		if (shopTable)
		{
			shopTable->SetWorldCoord(tablePositions[i]);
			shopTable->Update();
			AddRoomObject(shopTable);

			// 將商店桌子加入列表以便後續管理
			if (auto shopTablePtr = std::dynamic_pointer_cast<ShopTable>(shopTable))
			{
				m_ShopTables.push_back(shopTablePtr);
			}
		}
	}

	LOG_DEBUG("Created {} shop tables", tablePositions.size());
}

void ShopRoom::CreateMerchantNPC()
{
	// TODO: 之後實作商人NPC
	// 商人會在房間中央稍後的位置
	// 負責發送重置商品的事件
	LOG_DEBUG("Merchant NPC creation - to be implemented");
}

void ShopRoom::RefreshAllItems()
{
	LOG_DEBUG("Refreshing all shop items");

	// 先清理所有未購買的商品
	for (auto& shopTable : m_ShopTables)
	{
		if (shopTable)// && shopTable->HasItem())
		{
			shopTable->ClearCurrentItem();
		}
	}

	// 重新創建所有商品
	const glm::vec2 roomCenter = m_RoomSpaceInfo.m_WorldCoord;

	// 四個商品桌子的位置（與 CreateShopTables 保持一致）
	std::vector<glm::vec2> tablePositions = {
		roomCenter + glm::vec2(-93.0f, -49.0f), // 左下
		roomCenter + glm::vec2(-31.0f, -49.0f), // 左上
		roomCenter + glm::vec2(31.0f, -49.0f), // 右上
		roomCenter + glm::vec2(93.0f, -49.0f) // 右下
	};

	for (size_t i = 0; i < m_ShopTables.size() && i < tablePositions.size(); ++i)
	{
		auto& shopTable = m_ShopTables[i];
		if (!shopTable) continue;

		ShopItemType itemType;
		int price;
		PotionSize potionSize = PotionSize::SMALL;

		// 根據位置決定商品類型和價格（重新隨機化藥水大小）
		switch (i)
		{
		case 0: // 生命藥水 - 隨機大小
			{
				itemType = ShopItemType::HEALTH_POTION;
				// 50% 機率生成大藥水，50% 機率生成小藥水
				bool isBigPotion = (rand() % 2 == 0);
				price = isBigPotion ? 20 : 10; // 大藥水價格是小藥水的2倍
				potionSize = isBigPotion ? PotionSize::BIG : PotionSize::SMALL;
				break;
			}
		case 1: // 能量藥水 - 隨機大小
			{
				itemType = ShopItemType::ENERGY_POTION;
				// 50% 機率生成大藥水，50% 機率生成小藥水
				bool isBigPotion = (rand() % 2 == 0);
				price = isBigPotion ? 16 : 8; // 大藥水價格是小藥水的2倍
				potionSize = isBigPotion ? PotionSize::BIG : PotionSize::SMALL;
				break;
			}
		case 2: // 武器1
		case 3: // 武器2
			itemType = ShopItemType::WEAPON;
			price = 50;
			potionSize = PotionSize::SMALL; // 對武器無效
			break;
		}

		// 更新商店桌子的屬性
		shopTable->SetItemType(itemType);
		shopTable->SetPrice(price);
		shopTable->SetPotionSize(potionSize);

		// 使用工廠創建新商品
		std::shared_ptr<nGameObject> newItem = nullptr;

		switch (itemType)
		{
		case ShopItemType::HEALTH_POTION:
			newItem = m_Factory.lock()->CreatePotion(PotionType::HEALTH, potionSize);
			break;
		case ShopItemType::ENERGY_POTION:
			newItem = m_Factory.lock()->CreatePotion(PotionType::ENERGY, potionSize);
			break;
		case ShopItemType::WEAPON:
			{
				// 獲取玩家現有武器ID列表，避免重複
				std::vector<int> allPlayerWeaponID;
				if (auto player = m_Player.lock())
				{
					if (auto attackComp = player->GetComponent<AttackComponent>(ComponentType::ATTACK))
					{
						allPlayerWeaponID = attackComp->GetAllWeaponID();
					}
				}

				// 創建隨機武器
				newItem = WeaponFactory::createRandomWeapon(allPlayerWeaponID);
				break;
			}
		}

		if (newItem)
		{
			// 設置商品位置
			glm::vec2 itemOffset = glm::vec2(0, -20); // 商品在桌子上方20像素
			newItem->SetWorldCoord(tablePositions[i] + itemOffset);
			newItem->SetZIndexType(UI);

			// 添加到房間
			AddRoomObject(newItem);

			// 設置商品到商店桌子
			shopTable->SetCurrentItem(newItem);

			// 對於藥水，停用 InteractableComponent（購買前不能直接拾取）
			// if (itemType != ShopItemType::WEAPON)
			// {
				if (auto itemInteractable = newItem->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
				{
					itemInteractable->SetComponentActive(false);
				}
			// }

			// 更新商店桌子的價格提示
			if (auto interactableComp = shopTable->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
			{
				std::string priceText = std::to_string(price) + " 金幣";
				interactableComp->SetPromptText(priceText);
			}
		}
	}

	LOG_DEBUG("Successfully refreshed all shop items");
}
