//
// Created by QuzzS on 2025/4/25.
//

#include "Room/BossRoom.hpp"
#include "Components/DoorComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InteractableComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "ImagePoolManager.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "UIPanel/UISlider.hpp"
#include "Util/Color.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"
#include "config.hpp"

void BossRoom::Start(const std::shared_ptr<Character> &player)
{
	DungeonRoom::Start(player);

	// 初始化戰鬥管理器
	m_CombatManager.Initialize();

	// 創建傳送門（初始隱藏）
	CreatePortal();

	// 創建寶箱（初始隱藏）
	CreateTreasureChests();

	// 初始化Boss血量UI
	m_BossHealthUI.Initialize();
}

void BossRoom::Update()
{
	DungeonRoom::Update();
	m_CombatManager.Update();
	m_BossHealthUI.Update();
}

void BossRoom::LoadFromJSON()
{
	const nlohmann::ordered_json jsonData = m_Loader.lock()->LoadBossRoomObjectPosition();
	InitializeRoomObjects(jsonData);
}

void BossRoom::TryActivateByPlayer()
{
	switch (m_State)
	{
	case RoomState::UNEXPLORED:
		SetState(RoomState::COMBAT);
		break;
	case RoomState::COMBAT:
		if (m_CombatManager.IsCompleted())
		{
			SetState(RoomState::EXPLORED);
		}
		break;
	default:
		break;
	}
}

void BossRoom::OnStateChanged()
{
	switch (m_State)
	{
	case RoomState::COMBAT:
		CloseDoors();
		m_CombatManager.StartCombat();
		break;

	case RoomState::EXPLORED:
		OpenDoors();
		ShowPortalAndChests();
		m_BossHealthUI.Hide();
		break;

	default:
		LOG_DEBUG("BossRoom::OnStateChanged: No action needed for state {}", static_cast<int>(m_State));
		break;
	}
}

void BossRoom::OnEnemyDeathEvent(const EnemyDeathEvent &event)
{
	// 檢查死亡的敵人是否是Boss
	if (auto deadEnemy = event.m_DeadEnemy.lock())
	{
		if (deadEnemy->GetID() == 101)
		{
			m_CombatManager.OnBossDied();
		}
	}
}

void BossRoom::AddEnemy(const std::shared_ptr<Character> &enemy)
{
	if (!enemy)
	{
		LOG_WARN("BossRoom::AddEnemy - Attempted to add null enemy");
		return;
	}

	LOG_DEBUG("BossRoom::AddEnemy - Adding enemy: {}", enemy->GetName());

	// 設置敵人為活躍狀態
	enemy->SetActive(true);

	// 使用Room API添加敵人
	SpawnEntity(enemy, EntityCategory::ENEMY);

	// 如果是Boss，設置到戰鬥管理器和UI管理器
	if (enemy->GetName() == "Boss" || enemy->GetID() == 101)
	{
		m_BossHealthUI.SetBoss(enemy);
		LOG_DEBUG("BossRoom::AddEnemy - Boss set to UI manager");
	}
}

void BossRoom::CreatePortal()
{
	const auto portal = m_Factory.lock()->CreateRoomObject("object_transferBossGate_0", "portal");
	if (!portal)
		return;

	portal->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);

	// 確保 InteractableComponent 存在
	const auto interactionComp = portal->GetComponent<InteractableComponent>(ComponentType::INTERACTABLE);
	if (!interactionComp)
		return;

	// 安全設置提示物件位置
	if (const auto promptObj = interactionComp->GetPromptObject())
	{
		glm::vec2 portalSize = portal->GetImageSize();
		promptObj->SetWorldCoord(portal->GetWorldCoord());
	}

	// 初始設置為隱藏和不可控制
	portal->SetActive(false);
	portal->SetControlVisible(false);

	m_Portal = portal;
	AddRoomObject(portal);
}

void BossRoom::CreateTreasureChests()
{
	const auto factory = m_Factory.lock();
	if (!factory)
		return;

	// 計算寶箱位置（在傳送門下方並排）
	glm::vec2 portalPos = m_RoomSpaceInfo.m_WorldCoord;
	glm::vec2 chestOffset = glm::vec2(0.0f, -50.0f); // 在傳送門下方50像素

	// 創建武器寶箱
	m_WeaponChest = factory->CreateRoomObject("object_chestWeapon", "weaponChest");
	if (m_WeaponChest)
	{
		m_WeaponChest->SetWorldCoord(portalPos + chestOffset + glm::vec2(-25.0f, 0.0f)); // 左側
		m_WeaponChest->SetActive(false);
		m_WeaponChest->SetControlVisible(false);
		AddRoomObject(m_WeaponChest);
	}

	// 創建獎勵寶箱
	m_RewardChest = factory->CreateRoomObject("object_chestReward", "rewardChest");
	if (m_RewardChest)
	{
		m_RewardChest->SetWorldCoord(portalPos + chestOffset + glm::vec2(25.0f, 0.0f)); // 右側
		m_RewardChest->SetActive(false);
		m_RewardChest->SetControlVisible(false);
		AddRoomObject(m_RewardChest);
	}
}

void BossRoom::CloseDoors()
{
	for (const auto &door : m_Doors)
	{
		if (const auto doorComp = door->GetComponent<DoorComponent>(ComponentType::DOOR))
		{
			doorComp->DoorClosed();
		}
	}
}

void BossRoom::OpenDoors()
{
	for (const auto &door : m_Doors)
	{
		if (const auto doorComp = door->GetComponent<DoorComponent>(ComponentType::DOOR))
		{
			doorComp->DoorOpened();
		}
	}
}

void BossRoom::ShowPortalAndChests()
{
	// 顯示傳送門
	if (m_Portal)
	{
		m_Portal->SetActive(true);
		m_Portal->SetControlVisible(true);
	}

	// 顯示寶箱
	if (m_WeaponChest)
	{
		m_WeaponChest->SetActive(true);
		m_WeaponChest->SetControlVisible(true);
	}

	if (m_RewardChest)
	{
		m_RewardChest->SetActive(true);
		m_RewardChest->SetControlVisible(true);
	}
}

// === CombatManager 實作 ===
BossRoom::CombatManager::CombatManager(BossRoom *room) : m_Room(room) {}

void BossRoom::CombatManager::Initialize() { m_CombatState = CombatState::INACTIVE; }

void BossRoom::CombatManager::Update()
{
	if (m_CombatState == CombatState::WAVE_ACTIVE)
	{
		CheckBossCompletion();
	}
}

void BossRoom::CombatManager::StartCombat()
{
	LOG_INFO("Boss combat started!");
	m_CombatState = CombatState::WAVE_ACTIVE;
	SpawnBoss();
}

void BossRoom::CombatManager::EndCombat()
{
	LOG_INFO("Boss combat ended!");
	m_CombatState = CombatState::COMPLETED;
}

void BossRoom::CombatManager::SpawnBoss()
{
	// 創建Boss（這裡使用一個示例Boss ID，您可以根據需要調整）
	auto boss = CharacterFactory::GetInstance().createEnemy(101); // 假設1001是Boss的ID

	if (!boss)
	{
		LOG_ERROR("Failed to create boss!");
		return;
	}

	// 設置Boss位置（房間中央偏上）
	glm::vec2 bossPosition = m_Room->m_RoomSpaceInfo.m_WorldCoord + glm::vec2(0.0f, 80.0f);
	boss->SetWorldCoord(bossPosition);

	// 添加Boss到房間
	m_Room->SpawnEntity(boss);
	m_Boss = boss;

	// 設置Boss血量UI
	m_Room->m_BossHealthUI.SetBoss(boss);
	m_Room->m_BossHealthUI.Show();

	LOG_INFO("Boss spawned at position ({}, {})", bossPosition.x, bossPosition.y);
}

void BossRoom::CombatManager::OnBossDied()
{
	LOG_INFO("Boss has been defeated!");
	EndCombat();
}

void BossRoom::CombatManager::CheckBossCompletion()
{
	if (auto boss = m_Boss.lock())
	{
		// 檢查Boss是否還活著
		if (const auto healthComp = boss->GetComponent<HealthComponent>(ComponentType::HEALTH))
		{
			if (healthComp->GetCurrentHp() <= 0)
			{
				OnBossDied();
			}
		}
	}
	else
	{
		// Boss已經被銷毀，戰鬥結束
		OnBossDied();
	}
}

// === BossHealthUIManager 實作 ===
BossRoom::BossHealthUIManager::BossHealthUIManager(BossRoom *room) : m_Room(room) {}

BossRoom::BossHealthUIManager::~BossHealthUIManager() {}

void BossRoom::BossHealthUIManager::Initialize()
{
	// Boss血量監聽函數
	m_ListenBossHealth = [this]() -> float
	{
		if (auto boss = m_Boss)
		{
			if (const auto healthComp = boss->GetComponent<HealthComponent>(ComponentType::HEALTH))
			{
				const auto maxHP = static_cast<float>(healthComp->GetMaxHp());
				if (maxHP <= 0.0f)
					return 0.0f;
				return static_cast<float>(healthComp->GetCurrentHp()) / maxHP;
			}
		}
		return 0.0f;
	};

	// 創建Boss名稱文字
	m_BossNameText = std::make_shared<nGameObject>();
	m_BossNameText->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/ariblk.ttf", 24, "Boss",
																		Util::Color::FromRGB(255, 255, 255), false));
	m_BossNameText->SetZIndex(90.0f);
	m_BossNameText->m_Transform.scale = glm::vec2(UI_SCALE, UI_SCALE);

	// 設置Boss名稱位置（螢幕上方中央）
	glm::vec2 screenTopCenter =
		glm::vec2(0.0f, (static_cast<float>(PTSD_Config::WINDOW_HEIGHT) - m_BossNameText->GetScaledSize().y) / 2.0f);
	m_BossNameText->m_Transform.translation = screenTopCenter;
	m_UIObjects.push_back(m_BossNameText);

	// 創建血量軌道
	m_HealthTrack = std::make_shared<nGameObject>();
	m_HealthTrack->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/ui_playerStatus/track_hp.png"));
	m_UIObjects.push_back(m_HealthTrack);

	// 創建血量滑桿
	m_BossHealthSlider = std::make_shared<UISlider>(m_ListenBossHealth, m_HealthTrack, glm::vec2(3.0f, 4.0f), false);
	m_BossHealthSlider->SetDrawable(
		ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/groove_BossStatus.png"));
	m_BossHealthSlider->SetZIndex(91.0f);
	m_BossHealthSlider->m_Transform.scale = glm::vec2(UI_SCALE, UI_SCALE);

	// 設置血量條位置（在Boss名稱下方）
	m_BossHealthSlider->m_Transform.translation = screenTopCenter + glm::vec2(0.0f, -40.0f);
	m_BossHealthSlider->Start();
	m_UIObjects.push_back(m_BossHealthSlider);

	// 初始隱藏所有UI
	Hide();
}

void BossRoom::BossHealthUIManager::Update()
{
	if (m_IsVisible && m_BossHealthSlider)
	{
		m_BossHealthSlider->Update();
	}
}

void BossRoom::BossHealthUIManager::Show()
{
	if (m_IsVisible)
		return;

	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene)
		return;

	const auto renderer = scene->GetRoot().lock();
	if (!renderer)
		return;

	// 添加所有UI物件到場景
	for (const auto &uiObj : m_UIObjects)
	{
		renderer->AddChild(uiObj);
	}

	m_IsVisible = true;
	LOG_DEBUG("Boss health UI shown");
}

void BossRoom::BossHealthUIManager::Hide()
{
	if (!m_IsVisible)
		return;

	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	if (!scene)
		return;

	const auto renderer = scene->GetRoot().lock();
	if (!renderer)
		return;

	// 從場景移除所有UI物件
	for (const auto &uiObj : m_UIObjects)
	{
		renderer->RemoveChild(uiObj);
	}

	m_IsVisible = false;
	LOG_DEBUG("Boss health UI hidden");
}

void BossRoom::BossHealthUIManager::SetBoss(const std::shared_ptr<Character> &boss)
{
	m_Boss = boss;

	// 更新Boss名稱
	if (m_BossNameText && boss)
	{
		auto nameTextDrawable = std::dynamic_pointer_cast<Util::Text>(m_BossNameText->GetDrawable());
		if (nameTextDrawable)
		{
			nameTextDrawable->SetText(boss->GetName()); // 假設Character有GetName方法
		}
	}
}
