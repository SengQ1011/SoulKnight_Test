//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/Dungeon_Scene.hpp"

#include <functional>

#include "Components/CollisionComponent.hpp"
#include "GameMechanism/TalentDatabase.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/InteractableComponent.hpp"
#include "Cursor.hpp"
#include "Loader.hpp"
#include "ObserveManager/InputManager.hpp"
#include "SaveManager.hpp"
#include "Scene/SceneManager.hpp"


#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "Components/InputComponent.hpp"
#include "Components/walletComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Room/DungeonMap.hpp"
#include "UIPanel/GameHUDPanel.hpp"
#include "UIPanel/PausePanel.hpp"
#include "UIPanel/PlayerStatusPanel.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIButton.hpp"
#include "UIPanel/UIManager.hpp"
#include "UIPanel/UIPanel.hpp"
#include "Util/Image.hpp"

// 簡單的回調函數用於暫停按鈕
void OpenPausePanelDungeon() { UIManager::GetInstance().ShowPanel("pause"); }

std::shared_ptr<DungeonScene> DungeonScene::s_PreGeneratedInstance = nullptr;

void DungeonScene::Start()
{
	LOG_DEBUG("Entering Game Scene");

	// 確保獲取場景數據
	if (!m_SceneData)
	{
		Scene::Download();
	}

	// 檢查數據是否成功獲取
	if (!m_SceneData)
	{
		LOG_ERROR("Failed to get scene data in DungeonScene::Start()");
		return;
	}

	m_Loader = std::make_shared<Loader>(m_ThemeName);

	// if(!m_OnDeathText)
	// {
	// 	m_OnDeathText = std::make_shared<nGameObject>();
	// 	m_OnDeathText->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR"/Font/zpix.ttf",36,"菜
	// 就多練",Util::Color(255,255,0))); 	m_OnDeathText->SetZIndex(100); 	m_OnDeathText->SetZIndexType(CUSTOM);
	// 	m_OnDeathText->SetControlVisible(false);
	// 	m_Root->AddChild(m_OnDeathText);
	// 	m_Camera->AddChild(m_OnDeathText);
	// }

	// 初始化關卡顯示元素
	InitializeStageText();
	InitializeStageIcon();

	// 创建并初始化玩家
	CreatePlayer();

	// 设置相机
	m_MapHeight = 5 * 35 * 16; // Dungeon 5個房間 35個方塊 16像素
	SetupCamera();

	// 設置工廠
	m_RoomObjectFactory = std::make_shared<RoomObjectFactory>(m_Loader);

	m_Map = std::make_shared<DungeonMap>(m_RoomObjectFactory, m_Loader, m_Player);
	m_Map->Start();

	InitUIManager();
	InitAudioManager();

	// 初始化场景管理器
	InitializeSceneManagers();

	FlushPendingObjectsToRendererAndCamera();

	// 更新游戲數據
	m_SceneData->isInGameProgress = true;
	if (m_SceneData->gameProgress.currentStage == 0)
		m_SceneData->gameProgress.currentStage++;
	m_SceneData->gameProgress.dungeonStartTime = Util::Time::GetElapsedTimeMs();
	Upload();
}

void DungeonScene::Update()
{
	// 先更新UI管理器，處理UI相關的輸入
	UIManager::GetInstance().Update();

	// 測試用：按P鍵顯示/隱藏暫停面板
	if (Util::Input::IsKeyDown(Util::Keycode::P))
	{
		UIManager::GetInstance().TogglePanel("pause");
	}

	// Input处理 - 當暫停面板都沒有顯示時處理遊戲輸入
	if (!UIManager::GetInstance().IsPanelVisible("pause"))
	{
		for (auto &[type, manager] : m_Managers)
			manager->Update();

		// 更新房间
		m_Map->Update();

		const std::shared_ptr<DungeonRoom> dungeonRoom = m_Map->GetCurrentRoom();
		if (dungeonRoom)
		{
			m_CurrentRoom = dungeonRoom;
			dungeonRoom->Update();

			dungeonRoom->DebugDungeonRoom();
		}

		if (m_textTimer > 0)
			m_textTimer -= Util::Time::GetDeltaTimeMs() / 1000.0f;
		else if (m_textTimer < 0)
		{
			m_textTimer = 0;
			m_stageText->SetVisible(false);
			m_stageIcon->SetVisible(false);
		}

		m_Camera->Update();
	}

	// 調試界面
	// DrawStageDebugUI();

	// 更新渲染器（渲染總是需要更新）
	m_Root->Update();
}

void DungeonScene::Exit()
{
	LOG_DEBUG("Game Scene exited");

	// 只在玩家死亡時才直接返回，不做任何處理
	if (m_IsPlayerDeath)
		return;

	// 保存游戲的進度（但不增加關卡數）
	auto cumulativeTime = Util::Time::GetElapsedTimeMs() - m_SceneData->gameProgress.dungeonStartTime;
	m_SceneData->gameProgress.cumulativeTime += cumulativeTime;

	if (m_Player)
	{
		SavePlayerInformation(m_Player);
		// 移除自動增加關卡數的邏輯
		// 關卡數只能在進入傳送門時增加
	}
}

Scene::SceneType DungeonScene::Change()
{
	// if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
	// 	return Scene::SceneType::Menu;
	if (!m_Player->IsActive())
	{
		m_IsPlayerDeath = true;
		m_timer += Util::Time::GetDeltaTimeMs() / 1000.0f;
		// m_OnDeathText->SetControlVisible(true);

		if (m_timer >= 2.0f)
		{
			m_timer = 0.0f;
			// m_OnDeathText->SetControlVisible(false);
			// 死亡時跳轉到結算場景進行結算
			return Scene::SceneType::Result;
		}
	}
	if (m_IsChange)
	{
		LOG_DEBUG("Change Main Menu");
		return Scene::SceneType::DungeonLoad;
	}
	return Scene::SceneType::Null;
}

void DungeonScene::GenerateStaticDungeon()
{
	s_PreGeneratedInstance = std::make_shared<DungeonScene>();
	s_PreGeneratedInstance->BuildDungeon(); // 真正重的初始化過程
}

std::shared_ptr<DungeonScene> DungeonScene::GetPreGenerated()
{
	if (!s_PreGeneratedInstance)
	{
		LOG_ERROR("s_PreGeneratedInstance is null! You forgot to call GenerateStaticDungeon?");
	}
	return s_PreGeneratedInstance;
}

void DungeonScene::ClearPreGenerated() { s_PreGeneratedInstance = nullptr; }

void DungeonScene::BuildDungeon()
{
	m_Loader = std::make_shared<Loader>(m_ThemeName);

	// 创建并初始化玩家
	CreatePlayer();

	// 设置相机
	m_MapHeight = 5 * 35 * 16; // Dungeon 5個房間 35個方塊 16像素
	SetupCamera();

	// 設置工廠
	m_RoomObjectFactory = std::make_shared<RoomObjectFactory>(m_Loader);

	m_Map = std::make_shared<DungeonMap>(m_RoomObjectFactory, m_Loader, m_Player);
	m_Map->Start();

	// 初始化场景管理器
	InitializeSceneManagers();
}

void DungeonScene::SetupCamera() const
{
	m_Camera->SetMapSize(m_MapHeight);
	m_Camera->SetFollowTarget(m_Player);
}

void DungeonScene::InitializeSceneManagers()
{
	// 添加管理器到场景
	AddManager(ManagerTypes::INPUT, std::make_shared<InputManager>());
	AddManager(ManagerTypes::ATTACK, std::make_shared<AttackManager>());

	auto inputManager = GetManager<InputManager>(ManagerTypes::INPUT);
	// 注册输入观察者
	inputManager->addObserver(m_Player->GetComponent<InputComponent>(ComponentType::INPUT));
	inputManager->addObserver(m_Camera);
}

void DungeonScene::InitUIManager()
{
	UIManager::GetInstance().ResetPanels();

	// 創建設定面板 - 最高優先級模態面板
	const auto settingPanel = std::make_shared<SettingPanel>();
	settingPanel->Start();
	UIManager::GetInstance().RegisterPanel("setting", std::static_pointer_cast<UIPanel>(settingPanel), 2, true);

	// 創建暫停面板 - 中等優先級模態面板
	const auto pausePanel =
		std::make_shared<PausePanel>(m_Player->GetComponent<TalentComponent>(ComponentType::TALENT));
	pausePanel->Start();
	UIManager::GetInstance().RegisterPanel("pause", std::static_pointer_cast<UIPanel>(pausePanel), 1, true);

	// 創建遊戲 HUD 面板 - 低優先級非模態面板
	const auto gameHUDPanel =
		std::make_shared<GameHUDPanel>(m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH),
									   m_Player->GetComponent<WalletComponent>(ComponentType::WALLET), m_Map);
	gameHUDPanel->Start();
	UIManager::GetInstance().RegisterPanel("gameHUD", std::static_pointer_cast<UIPanel>(gameHUDPanel), 0, false);
}

void DungeonScene::InitAudioManager()
{
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();
}

void DungeonScene::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);
	if (!m_Player)
		LOG_ERROR("Failed to create player");
	const auto playerData = m_SceneData->gameProgress.playerData;

	// Hp & 能量
	if (const auto healthComp = m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH))
	{
		const auto hp = playerData.currentHp;
		const auto energy = playerData.currentEnergy;
		healthComp->SetCurrentHp(hp);
		healthComp->SetCurrentEnergy(energy);
	}

	std::vector<Talent> talentDatabase = CreateTalentList(); // 創建天賦資料庫
	if (auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT))
	{
		// 武器
		if (const auto attackComp = m_Player->GetComponent<AttackComponent>(ComponentType::ATTACK))
		{
			// 移除初始武器
			attackComp->RemoveAllWeapon();
			auto weaponID = playerData.weaponID;
			for (const auto &id : weaponID)
			{
				attackComp->AddWeapon(WeaponFactory::createWeapon(id));
			}
		}

		// 恢復天賦
		std::vector<Talent> talentDatabase = CreateTalentList(); // 創建天賦資料庫
		if (const auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT))
		{
			auto talentID = playerData.talentID;
			for (const auto &talent : talentID)
				talentComp->AddTalent(talentDatabase[talent]);
		}

		// money
		if (const auto walletComp = m_Player->GetComponent<WalletComponent>(ComponentType::WALLET))
		{
			const auto player_money = playerData.money;
			walletComp->SetMoney(player_money);
		}

		// 设置玩家的初始位置
		m_Player->SetWorldCoord(glm::vec2(0)); // 地图正中央

		// 获取碰撞组件并添加到场景和相机
		if (const auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION))
		{
			// 将碰撞盒添加到场景根节点和相机
			const auto playerVisibleBox = collision->GetVisibleBox();
			m_PendingObjects.emplace_back(playerVisibleBox);
			playerVisibleBox->SetRegisteredToScene(true);
		}

		// 将玩家添加到场景根节点和相机
		m_PendingObjects.emplace_back(m_Player);
		m_Player->SetRegisteredToScene(true);
	}
}

void DungeonScene::DrawStageDebugUI()
{
	ImGui::Begin("Stage UI Debug");

	// === Stage Text Debug ===
	if (ImGui::CollapsingHeader("Stage Text Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 初始化 DebugPos（只做一次）
		static bool m_StageTextDebugInitialized = false;
		static glm::vec2 m_DebugStageTextPos;
		if (!m_StageTextDebugInitialized && m_stageText)
		{
			m_DebugStageTextPos = m_stageText->m_Transform.translation;
			m_StageTextDebugInitialized = true;
		}

		// 根據使用者輸入更新 Transform 狀態
		static bool m_StageTextPosChangedByInput = false;
		if (m_StageTextPosChangedByInput)
		{
			if (m_stageText)
				m_stageText->m_Transform.translation = m_DebugStageTextPos;
			m_StageTextPosChangedByInput = false;
		}

		// 位置調整
		ImGui::InputFloat("Stage Text x", &m_DebugStageTextPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			m_StageTextPosChangedByInput = true;

		ImGui::InputFloat("Stage Text y", &m_DebugStageTextPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			m_StageTextPosChangedByInput = true;

		// 不要每幀覆蓋 m_DebugStageTextPos，否則用戶輸入會被吃掉
		if (!m_StageTextPosChangedByInput && !ImGui::IsAnyItemActive() && m_stageText)
			m_DebugStageTextPos = m_stageText->m_Transform.translation;

		// 顯示/隱藏控制
		if (m_stageText)
		{
			bool isVisible = m_stageText->IsControlVisible();
			if (ImGui::Checkbox("Show Stage Text", &isVisible))
			{
				m_stageText->SetControlVisible(isVisible);
			}
		}
	}

	// === Stage Icon Debug ===
	if (ImGui::CollapsingHeader("Stage Icon Settings", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 初始化 DebugPos（只做一次）
		static bool m_StageIconDebugInitialized = false;
		static glm::vec2 m_DebugStageIconPos;
		if (!m_StageIconDebugInitialized && m_stageIcon)
		{
			m_DebugStageIconPos = m_stageIcon->m_Transform.translation;
			m_StageIconDebugInitialized = true;
		}

		// 根據使用者輸入更新 Transform 狀態
		static bool m_StageIconPosChangedByInput = false;
		if (m_StageIconPosChangedByInput)
		{
			if (m_stageIcon)
				m_stageIcon->m_Transform.translation = m_DebugStageIconPos;
			m_StageIconPosChangedByInput = false;
		}

		// 位置調整
		ImGui::InputFloat("Stage Icon x", &m_DebugStageIconPos.x, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			m_StageIconPosChangedByInput = true;

		ImGui::InputFloat("Stage Icon y", &m_DebugStageIconPos.y, 1.0f);
		if (ImGui::IsItemDeactivatedAfterEdit())
			m_StageIconPosChangedByInput = true;

		// 不要每幀覆蓋 m_DebugStageIconPos，否則用戶輸入會被吃掉
		if (!m_StageIconPosChangedByInput && !ImGui::IsAnyItemActive() && m_stageIcon)
			m_DebugStageIconPos = m_stageIcon->m_Transform.translation;

		// 顯示/隱藏控制
		if (m_stageIcon)
		{
			bool isVisible = m_stageIcon->IsControlVisible();
			if (ImGui::Checkbox("Show Stage Icon", &isVisible))
			{
				m_stageIcon->SetControlVisible(isVisible);
			}
		}
	}

	// === Timer Control ===
	if (ImGui::CollapsingHeader("Timer Control"))
	{
		ImGui::Text("Current Timer: %.2f", m_textTimer);
		ImGui::InputFloat("Text Timer", &m_textTimer, 0.1f);

		if (ImGui::Button("Reset Timer (Show UI)"))
		{
			m_textTimer = 3.0f; // 重設為3秒
			if (m_stageText)
				m_stageText->SetControlVisible(true);
			if (m_stageIcon)
				m_stageIcon->SetControlVisible(true);
		}

		ImGui::SameLine();
		if (ImGui::Button("Hide UI Now"))
		{
			m_textTimer = 0;
			if (m_stageText)
				m_stageText->SetControlVisible(false);
			if (m_stageIcon)
				m_stageIcon->SetControlVisible(false);
		}
	}

	ImGui::End();
}

void DungeonScene::InitializeStageText()
{
	if (m_stageText)
		return;

	const std::string stage = std::to_string(m_SceneData->gameProgress.currentChapter) + "-" +
		std::to_string(m_SceneData->gameProgress.currentStage);

	m_stageText = std::make_shared<nGameObject>();
	m_stageText->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR "/Font/pixel_bold.TTF", 24, stage,
																	 Util::Color(255, 255, 255), false));
	m_stageText->SetZIndex(100);
	m_stageText->SetZIndexType(CUSTOM);
	m_stageText->m_Transform.translation = glm::vec2(36.0f, 120.0f);
	m_stageText->m_Transform.scale = glm::vec2(4.0f);
	m_Root->AddChild(m_stageText);
}

void DungeonScene::InitializeStageIcon()
{
	if (m_stageIcon)
		return;

	m_stageIcon = std::make_shared<nGameObject>();
	m_stageIcon->SetDrawable(ImagePoolManager::GetInstance().GetImage(RESOURCE_DIR "/UI/stageTitle/stageTitle_2.png"));
	m_stageIcon->SetZIndex(100);
	m_stageIcon->SetZIndexType(CUSTOM);
	m_stageIcon->m_Transform.translation = glm::vec2(0.0f, 120.0f);
	m_stageIcon->m_Transform.scale = glm::vec2(4.0f);
	m_Root->AddChild(m_stageIcon);
}

void DungeonScene::OnStageCompleted()
{
	// 處理關卡完成時的邏輯
	if (m_SceneData)
	{
		m_SceneData->gameProgress.currentStage++;
		LOG_INFO("Stage completed! Current stage: {}", m_SceneData->gameProgress.currentStage);

		// 保存玩家資訊
		if (m_Player)
		{
			SavePlayerInformation(m_Player);
		}
	}
}
