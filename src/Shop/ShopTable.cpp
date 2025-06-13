//
// Created by AI Assistant on 2025/4/29.
//

#include "Shop/ShopTable.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InteractableComponent.hpp"
#include "Components/WalletComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/Dungeon_Scene.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Logger.hpp"


ShopTable::ShopTable(const std::string &baseName, const std::string &baseClass) : nGameObject(baseName, baseClass) {}


void ShopTable::Initialize(const std::string &name, ShopItemType itemType, int price, PotionSize potionSize)
{
	// 設置商店桌子的屬性
	m_ItemType = itemType;
	m_Price = price;
	m_PotionSize = potionSize;
}

void ShopTable::Update()
{
	nGameObject::Update();
	// 更新商品位置，讓商品跟隨桌子移動
	if (m_CurrentItem)
	{
		glm::vec2 tablePos = GetWorldCoord();
		glm::vec2 itemPos = tablePos + m_ItemOffset;
		m_CurrentItem->SetWorldCoord(itemPos);
	}
}

void ShopTable::CreateItem(const std::shared_ptr<Character> &player)
{
	if (m_CurrentItem)
	{
		LOG_DEBUG("ShopTable already has an item, removing old one");
		RemoveCurrentItem();
	}

	switch (m_ItemType)
	{
	case ShopItemType::HEALTH_POTION:
		CreateHealthPotion();
		break;
	case ShopItemType::ENERGY_POTION:
		CreateEnergyPotion();
		break;
	case ShopItemType::WEAPON:
		CreateRandomWeapon(player);
		break;
	}

	if (m_CurrentItem)
	{
		// 將商品設置在桌子上方
		m_CurrentItem->SetWorldCoord(GetWorldCoord() + m_ItemOffset);

		// 將商品的 InteractableComponent 設為不活躍（購買前不能直接拾取）
		if (auto itemInteractable = m_CurrentItem->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
		{
			itemInteractable->SetComponentActive(false);
		}

		LOG_DEBUG("Created item on shop table: {}", m_CurrentItem->GetName());
	}
}

void ShopTable::RefreshItem(const std::shared_ptr<Character> &player)
{
	LOG_DEBUG("Refreshing item on shop table");
	CreateItem(player);
}

bool ShopTable::CanPurchase(const std::shared_ptr<Character> &player) const
{
	if (!player || !m_CurrentItem)
		return false;

	auto walletComp = player->GetComponent<walletComponent>(ComponentType::WALLET);
	if (!walletComp)
		return false;

	return walletComp->GetMoney() >= m_Price;
}

bool ShopTable::PurchaseItem(const std::shared_ptr<Character> &player)
{
	if (!CanPurchase(player))
	{
		LOG_DEBUG("Cannot purchase item: insufficient funds or no item");
		return false;
	}

	// TODO: OnEvent Player
	auto walletComp = player->GetComponent<walletComponent>(ComponentType::WALLET);

	// 扣除金錢
	if (!walletComp->SpendMoney(m_Price))
	{
		LOG_DEBUG("Failed to spend money");
		return false;
	}

	AudioManager::GetInstance().PlaySFX("buy_sound");
	m_CurrentItem->SetActive(true);
	// 根據商品類型處理
	switch (m_ItemType)
	{
	case ShopItemType::HEALTH_POTION:
	case ShopItemType::ENERGY_POTION:
		{
			// 觸發商品的 InteractableComponent OnInteract 方法
			if (auto itemInteractable = m_CurrentItem->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
			{
				// 暫時啟用 InteractableComponent 以便觸發互動
				itemInteractable->SetComponentActive(true);
				// 觸發互動效果（使用配置好的 HP_POISON 或 ENERGY_POISON 邏輯）
				itemInteractable->OnInteract(player);
				LOG_DEBUG("Triggered potion interaction effect using InteractableComponent");
			}
			break;
		}
	case ShopItemType::WEAPON:
		{
			// 啟用武器的 InteractableComponent，讓玩家可以拾取
			if (auto itemInteractable = m_CurrentItem->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
			{
				itemInteractable->SetComponentActive(true);
				LOG_DEBUG("Weapon purchased, InteractableComponent activated for: {}", m_CurrentItem->GetName());
			}

			// 註冊武器到 RoomInteractionManager（現在有重複檢查機制）
			const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
			if (currentScene && currentScene->GetCurrentRoom())
			{
				if (auto interactionManager = currentScene->GetCurrentRoom()->GetInteractionManager())
				{
					interactionManager->RegisterInteractable(m_CurrentItem);
					LOG_DEBUG("Attempting to register weapon to InteractionManager: {}", m_CurrentItem->GetName());
				}
			}

			// 清空 m_CurrentItem，表示商品已售出
			m_CurrentItem.reset();
			break;
		}
	}

	// 對於藥水，購買後移除商品並關閉 ShopTable 互動
	if (m_ItemType != ShopItemType::WEAPON)
	{
		RemoveCurrentItem();
		m_CurrentItem.reset();
	}
	// 關閉 ShopTable 的 InteractableComponent，防止重複購買
	if (const auto interactableComp = GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		interactableComp->SetComponentActive(false);
		LOG_DEBUG("ShopTable InteractableComponent deactivated after weapon purchase");
	}

	LOG_INFO("Successfully purchased item for {} coins", m_Price);
	return true;
}

void ShopTable::CreateHealthPotion()
{
	// 根據藥水大小決定要創建的藥水類型
	std::string potionName;
	std::string imagePath;

	if (m_PotionSize == PotionSize::BIG)
	{
		potionName = "object_bigHealthPotion";
		imagePath = RESOURCE_DIR "/Items/object_bigHealthPotion.png";
	}
	else
	{
		potionName = "object_smallHealthPotion";
		imagePath = RESOURCE_DIR "/Items/object_smallHealthPotion.png";
	}

	m_CurrentItem = std::make_shared<nGameObject>(potionName);

	// 設置圖像
	auto drawable = ImagePoolManager::GetInstance().GetImage(imagePath);
	if (drawable)
	{
		m_CurrentItem->SetDrawable(drawable);
	}

	// 手動添加 InteractableComponent（模擬 JSON 配置的效果）
	auto interactableComp = m_CurrentItem->AddComponent<InteractableComponent>(
		ComponentType::INTERACTABLE, InteractableType::HP_POISON, nullptr, 10.0f, false);

	// 註冊到場景
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		scene->GetPendingObjects().emplace_back(m_CurrentItem);
		m_CurrentItem->SetRegisteredToScene(true);
	}
}

void ShopTable::CreateEnergyPotion()
{
	// 根據藥水大小決定要創建的藥水類型
	std::string potionName;
	std::string imagePath;

	if (m_PotionSize == PotionSize::BIG)
	{
		potionName = "object_bigEnergyPotion";
		imagePath = RESOURCE_DIR "/Items/object_bigEnergyPotion.png";
	}
	else
	{
		potionName = "object_smallEnergyPotion";
		imagePath = RESOURCE_DIR "/Items/object_smallEnergyPotion.png";
	}

	m_CurrentItem = std::make_shared<nGameObject>(potionName);

	// 設置圖像
	auto drawable = ImagePoolManager::GetInstance().GetImage(imagePath);
	if (drawable)
	{
		m_CurrentItem->SetDrawable(drawable);
	}

	// 手動添加 InteractableComponent（模擬 JSON 配置的效果）
	auto interactableComp = m_CurrentItem->AddComponent<InteractableComponent>(
		ComponentType::INTERACTABLE, InteractableType::ENERGY_POISON, nullptr, 10.0f, false);

	// 註冊到場景
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		scene->GetPendingObjects().emplace_back(m_CurrentItem);
		m_CurrentItem->SetRegisteredToScene(true);
	}
}

void ShopTable::CreateRandomWeapon(const std::shared_ptr<Character> &player)
{
	// 獲取玩家現有武器ID列表，避免重複
	std::vector<int> allPlayerWeaponID;
	if (auto attackComp = player->GetComponent<AttackComponent>(ComponentType::ATTACK))
	{
		allPlayerWeaponID = attackComp->GetAllWeaponID();
	}

	// 創建隨機武器
	auto weapon = WeaponFactory::createRandomWeapon(allPlayerWeaponID);
	if (!weapon)
	{
		LOG_ERROR("Failed to create random weapon for shop");
		return;
	}

	m_CurrentItem = weapon;

	// 註冊到場景
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		scene->GetRoot().lock()->AddChild(weapon);
		scene->GetCamera().lock()->SafeAddChild(weapon);
		weapon->SetRegisteredToScene(true);
		weapon->SetControlVisible(true);
	}
}

void ShopTable::RemoveCurrentItem()
{
	if (!m_CurrentItem)
		return;

	// 從場景中移除
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (scene)
	{
		// scene->GetRoot().lock()->RemoveChild(m_CurrentItem);
		// scene->GetCamera().lock()->MarkForRemoval(m_CurrentItem);
		auto room = scene->GetCurrentRoom();
		// m_CurrentItem->SetControlVisible(false);
		// m_CurrentItem->SetVisible(false);
		room->RemoveRoomObject(m_CurrentItem);
	}

	m_CurrentItem.reset();
	LOG_DEBUG("Removed current item from shop table");
}

void ShopTable::ClearCurrentItem()
{
	// 移除當前商品（如果存在）
	RemoveCurrentItem();

	// m_CurrentItem = nullptr;
	// 重新啟用商店桌子的 InteractableComponent（如果之前被停用）
	if (auto interactableComp = GetComponent<InteractableComponent>(ComponentType::INTERACTABLE))
	{
		interactableComp->SetComponentActive(true);
		LOG_DEBUG("Re-enabled ShopTable InteractableComponent for refresh");
	}
}
