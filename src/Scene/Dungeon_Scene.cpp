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
#include "Room/MonsterRoom.hpp"
#include "Room/MonsterRoomTestUI.hpp"
#include "Structs/EventInfo.hpp"
#include "UIPanel/GameHUDPanel.hpp"
#include "UIPanel/KeyPanel.hpp"
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

	// 初始化測試UI
	InitializeMonsterRoomTestUI();

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
	// if (m_Player->IsActive()) UIManager::GetInstance().Update();
	UIManager::GetInstance().Update();

	// Input处理 - 當暫停面板都沒有顯示時處理遊戲輸入
	if (!UIManager::GetInstance().IsPanelVisible("pause"))
	{
		// 處理佈局更換相關輸入和UI
		HandleLayoutChangeInput();

		for (auto &[type, manager] : m_Managers)
			manager->Update();

		// 更新房间
		m_Map->Update();

		const std::shared_ptr<DungeonRoom> dungeonRoom = m_Map->GetCurrentRoom();
		if (dungeonRoom)
		{
			m_CurrentRoom = dungeonRoom;
			dungeonRoom->Update();

			// dungeonRoom->DebugDungeonRoom();
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
	DrawGameDebugUI();

	// 更新渲染器（渲染總是需要更新）
	m_Root->Update();

	if (m_PendingObjects.size() > 0)
		FlushPendingObjectsToRendererAndCamera();

	// 在渲染完成後觸發玩家ShowUp事件（只觸發一次）
	if (!m_PlayerShowUpTriggered && m_Player)
	{
		TriggerPlayerShowUp();
		m_PlayerShowUpTriggered = true;
	}
}

void DungeonScene::Exit()
{
	LOG_DEBUG("Game Scene exited");

	// 保存游戲的進度（但不增加關卡數）
	auto cumulativeTime = Util::Time::GetElapsedTimeMs() - m_SceneData->gameProgress.dungeonStartTime;
	m_SceneData->gameProgress.cumulativeTime += cumulativeTime;

	if (m_Player)
	{
		SavePlayerInformation(m_Player);
		// 移除自動增加關卡數的邏輯
		// 關卡數只能在進入傳送門時增加
	}

	// 如果是玩家死亡，更新存檔狀態但不增加關卡進度
	if (m_IsPlayerDeath && m_SceneData)
	{
		LOG_DEBUG("Player died, saving progress without stage advancement");
		// 上傳更新的數據（包含killCount和playerData等）
		auto &sceneManager = SceneManager::GetInstance();
		sceneManager.UploadGameProgress(m_SceneData);
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

	// 創建按鍵說明面板 - 最高優先級模態面板
	const auto keyPanel = std::make_shared<KeyPanel>();
	keyPanel->Start();
	UIManager::GetInstance().RegisterPanel("key", std::static_pointer_cast<UIPanel>(keyPanel), 2, true);

	// 創建暫停面板 - 中等優先級模態面板
	const auto pausePanel =
		std::make_shared<PausePanel>(m_Player->GetComponent<TalentComponent>(ComponentType::TALENT));
	pausePanel->Start();
	UIManager::GetInstance().RegisterPanel("pause", std::static_pointer_cast<UIPanel>(pausePanel), 1, true);

	// 創建遊戲 HUD 面板 - 低優先級非模態面板
	const auto gameHUDPanel =
		std::make_shared<GameHUDPanel>(m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH),
									   m_Player->GetComponent<walletComponent>(ComponentType::WALLET), m_Map);
	gameHUDPanel->Start();
	UIManager::GetInstance().RegisterPanel("gameHUD", std::static_pointer_cast<UIPanel>(gameHUDPanel), 0, false);
}

void DungeonScene::InitAudioManager()
{
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM("dungeon");
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
		if (const auto walletComp = m_Player->GetComponent<walletComponent>(ComponentType::WALLET))
		{
			const auto player_money = playerData.money;
			walletComp->SetMoney(player_money);
		}

		// 设置玩家的初始位置
		m_Player->SetWorldCoord(glm::vec2(0)); // 地图正中央

		// 获取碰撞组件并添加到场景和相机
		if (const auto collision = m_Player->GetComponent<CollisionComponent>(ComponentType::COLLISION))
		{
			// 碰撞箱可視化功能已移除
			/*
			// 将碰撞盒添加到场景根节点和相机
			const auto playerVisibleBox = collision->GetVisibleBox();
			m_PendingObjects.emplace_back(playerVisibleBox);
			playerVisibleBox->SetRegisteredToScene(true);
			*/
		}

		// 将玩家添加到场景根节点和相机
		m_PendingObjects.emplace_back(m_Player);
		m_Player->SetRegisteredToScene(true);

		// 初始設置玩家為不可見，等待ShowUp事件
		m_Player->SetControlVisible(false);
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

void DungeonScene::DrawGameDebugUI()
{
	ImGui::Begin("Game Debug Console");

	// === 玩家錢包調試 ===
	if (ImGui::CollapsingHeader("Player Wallet", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_Player)
		{
			if (auto walletComp = m_Player->GetComponent<walletComponent>(ComponentType::WALLET))
			{
				ImGui::Text("Current Money: %d", walletComp->GetMoney());

				// Money adjustment slider
				int tempMoney = walletComp->GetMoney();
				if (ImGui::SliderInt("Set Money", &tempMoney, 0, 9999))
				{
					walletComp->SetMoney(tempMoney);
				}

				// Quick adjustment buttons
				if (ImGui::Button("+ 100"))
				{
					walletComp->AddMoney(100);
				}
				ImGui::SameLine();
				if (ImGui::Button("+ 500"))
				{
					walletComp->AddMoney(500);
				}
				ImGui::SameLine();
				if (ImGui::Button("+ 1000"))
				{
					walletComp->AddMoney(1000);
				}

				// Second row buttons
				if (ImGui::Button("Reset"))
				{
					walletComp->SetMoney(0);
				}
				ImGui::SameLine();
				if (ImGui::Button("Max Value"))
				{
					walletComp->SetMoney(9999);
				}

				// Custom amount addition
				static int addAmount = 50;
				ImGui::InputInt("Custom Amount", &addAmount);
				if (ImGui::Button("Add Custom Amount"))
				{
					walletComp->AddMoney(addAmount);
				}
			}
		}
	}

	// === 玩家血量調試 ===
	if (ImGui::CollapsingHeader("Player Health", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_Player)
		{
			if (auto healthComp = m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH))
			{
				ImGui::Text("Health: %d / %d", healthComp->GetCurrentHp(), healthComp->GetMaxHp());

				if (ImGui::Button("Full Health"))
				{
					healthComp->SetCurrentHp(healthComp->GetMaxHp());
				}
				ImGui::SameLine();
				if (ImGui::Button("Half Health"))
				{
					healthComp->SetCurrentHp(healthComp->GetMaxHp() / 2);
				}
				ImGui::SameLine();
				if (ImGui::Button("Low Health"))
				{
					healthComp->SetCurrentHp(1);
				}
			}
		}
	}

	// === MonsterRoom 調試 ===
	if (ImGui::CollapsingHeader("Monster Room", ImGuiTreeNodeFlags_DefaultOpen))
	{
		if (m_Map)
		{
			auto currentRoom = m_Map->GetCurrentRoom();
			if (auto monsterRoom = std::dynamic_pointer_cast<MonsterRoom>(currentRoom))
			{
				// 顯示房間基本信息
				const char *stateNames[] = {"UNEXPLORED", "COMBAT", "EXPLORED"};
				ImGui::Text("Room State: %s", stateNames[static_cast<int>(monsterRoom->GetState())]);
				ImGui::Text("Grid Position: (%.0f, %.0f)", monsterRoom->GetMapGridPos().x,
							monsterRoom->GetMapGridPos().y);

				ImGui::Separator();

				// 戰鬥控制按鈕
				if (ImGui::Button("Kill All Enemies"))
				{
					monsterRoom->DebugKillAllEnemies();
				}
				ImGui::SameLine();
				if (ImGui::Button("Kill Current Wave"))
				{
					monsterRoom->DebugKillCurrentWave();
				}

				// 佈局更換功能
				ImGui::Separator();
				ImGui::Text("Layout Control:");
				if (ImGui::Button("Change to Random Layout"))
				{
					monsterRoom->ChangeLayoutRuntime("RANDOM");
				}
			}
			else
			{
				ImGui::Text("Current room is not a MonsterRoom");
			}
		}
		else
		{
			ImGui::Text("No map available");
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
		// TODO： 如果有第二章節以上這裏要再改改
		const auto currentStage = m_SceneData->gameProgress.currentStage;
		if (currentStage < 5)
		{
			m_SceneData->gameProgress.currentStage++;
		}
		LOG_INFO("Stage completed! Current stage: {}", m_SceneData->gameProgress.currentStage);

		// 保存玩家資訊
		if (m_Player)
		{
			SavePlayerInformation(m_Player);
		}
	}
}

void DungeonScene::InitializeMonsterRoomTestUI()
{
	// 暫時為空，等待當前房間變為 MonsterRoom 時再初始化
	m_MonsterRoomTestUI = nullptr;
	LOG_DEBUG("MonsterRoomTestUI initialized (deferred)");
}

void DungeonScene::HandleLayoutChangeInput()
{
	// 更新冷卻時間
	if (m_LayoutChangeCooldown > 0.0f)
	{
		m_LayoutChangeCooldown -= Util::Time::GetDeltaTimeMs() / 1000.0f;
	}

	// 檢查當前房間是否為 MonsterRoom
	if (auto currentRoom = m_Map->GetCurrentRoom())
	{
		if (auto monsterRoom = std::dynamic_pointer_cast<MonsterRoom>(currentRoom))
		{
			// 如果 UI 尚未創建，現在創建它
			if (!m_MonsterRoomTestUI)
			{
				m_MonsterRoomTestUI = std::make_shared<MonsterRoomTestUI>(monsterRoom);
				LOG_DEBUG("MonsterRoomTestUI created for current MonsterRoom");
			}

			// 渲染 UI
			m_MonsterRoomTestUI->RenderUI();

			// 處理佈局更換請求
			ProcessLayoutChangeRequest();
		}
		else
		{
			// 如果當前房間不是 MonsterRoom，清理 UI
			if (m_MonsterRoomTestUI)
			{
				m_MonsterRoomTestUI = nullptr;
				LOG_DEBUG("MonsterRoomTestUI cleared (not in MonsterRoom)");
			}
		}
	}
}

void DungeonScene::ProcessLayoutChangeRequest()
{
	if (!m_MonsterRoomTestUI || !m_MonsterRoomTestUI->HasLayoutChangeRequest())
		return;

	// 檢查冷卻時間
	if (m_LayoutChangeCooldown > 0.0f)
	{
		LOG_DEBUG("Layout change on cooldown, remaining: {:.2f}s", m_LayoutChangeCooldown);
		m_MonsterRoomTestUI->ClearLayoutChangeRequest();
		return;
	}

	std::string requestedLayout = m_MonsterRoomTestUI->GetRequestedLayout();
	ChangeCurrentRoomLayout(requestedLayout);

	// 清除請求並設置冷卻時間
	m_MonsterRoomTestUI->ClearLayoutChangeRequest();
	m_LayoutChangeCooldown = 1.0f; // 1秒冷卻時間

	LOG_INFO("Layout change completed, cooldown activated");
}

void DungeonScene::ChangeCurrentRoomLayout(const std::string &layoutName)
{
	auto currentRoom = m_Map->GetCurrentRoom();
	if (!currentRoom)
	{
		LOG_WARN("No current room available for layout change");
		return;
	}

	auto monsterRoom = std::dynamic_pointer_cast<MonsterRoom>(currentRoom);
	if (!monsterRoom)
	{
		LOG_WARN("Current room is not a MonsterRoom, cannot change layout");
		return;
	}

	if (!monsterRoom->CanChangeLayout())
	{
		LOG_WARN("MonsterRoom is in combat state, cannot change layout");
		return;
	}

	try
	{
		// 執行佈局更換
		monsterRoom->ChangeLayoutRuntime(layoutName);
		LOG_INFO("Successfully changed room layout to: {}", layoutName.empty() ? "random" : layoutName);
	}
	catch (const std::exception &e)
	{
		LOG_ERROR("Failed to change room layout: {}", e.what());
	}
}

void DungeonScene::TriggerPlayerShowUp()
{
	if (!m_Player)
	{
		LOG_WARN("DungeonScene::TriggerPlayerShowUp: Player is null");
		return;
	}

	// 創建ShowUp事件並發送給玩家
	ShowUpEvent showUpEvent;
	m_Player->OnEvent(showUpEvent);

	LOG_DEBUG("Player ShowUp event triggered");
}
