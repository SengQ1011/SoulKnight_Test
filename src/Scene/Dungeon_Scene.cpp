//
// Created by QuzzS on 2025/3/4.
//


#include "Scene/Dungeon_Scene.hpp"

#include "Components/CollisionComponent.hpp"
#include "GameMechanism/TalentDatabase.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/InteractableComponent.hpp"
#include "Cursor.hpp"
#include "Loader.hpp"
#include "Scene/SceneManager.hpp"
#include "SaveManager.hpp"
#include "ObserveManager/InputManager.hpp"
#include "Scene/SceneManager.hpp"


#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"

#include "Components/InputComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Factory/RoomObjectFactory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Room/DungeonMap.hpp"
#include "UIPanel/PlayerStatusPanel.hpp"
#include "UIPanel/SettingPanel.hpp"
#include "UIPanel/UIManager.hpp"
#include "Util/Image.hpp"

std::shared_ptr<DungeonScene> DungeonScene::s_PreGeneratedInstance = nullptr;

void DungeonScene::Start()
{
	LOG_DEBUG("Entering Game Scene");
	m_Loader = std::make_shared<Loader>(m_ThemeName);

	// if(!m_OnDeathText)
	// {
	// 	m_OnDeathText = std::make_shared<nGameObject>();
	// 	m_OnDeathText->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR"/Font/zpix.ttf",36,"菜 就多練",Util::Color(255,255,0)));
	// 	m_OnDeathText->SetZIndex(100);
	// 	m_OnDeathText->SetZIndexType(CUSTOM);
	// 	m_OnDeathText->SetControlVisible(false);
	// 	m_Root->AddChild(m_OnDeathText);
	// 	m_Camera->AddChild(m_OnDeathText);
	// }
	const std::string stage = "Stage"+ std::to_string(m_SceneData->gameProgress.currentChapter) + "-" +
					std::to_string(m_SceneData->gameProgress.currentStage);

	if(!m_stageText) {
		m_stageText = std::make_shared<nGameObject>();
		m_stageText->SetDrawable(ImagePoolManager::GetInstance().GetText(RESOURCE_DIR"/Font/BRUSHSCI.TTF",32,stage,Util::Color(255,255,255)));
		m_stageText->SetZIndex(100);
		m_stageText->SetZIndexType(CUSTOM);
		m_stageText->SetControlVisible(true);
		m_Root->AddChild(m_stageText);
		m_Camera->AddChild(m_stageText);
	}

	// 创建并初始化玩家
	CreatePlayer();

	// 设置相机
	m_MapHeight = 5 * 35 * 16; // Dungeon 5個房間 35個方塊 16像素
	SetupCamera();

	//設置工廠
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
	if (m_SceneData->gameProgress.currentStage == 0) m_SceneData->gameProgress.currentStage++;
	m_SceneData->gameProgress.dungeonStartTime = Util::Time::GetElapsedTimeMs();
	Upload();
}

void DungeonScene::Update()
{
	for (auto &[type, manager] : m_Managers)
		manager->Update();

	UIManager::GetInstance().Update();
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
	else if(m_textTimer < 0)
	{
		m_textTimer = 0;
		m_stageText->SetControlVisible(false);
	}

	// 更新相机
	m_Camera->Update();

	// 更新渲染器
	m_Root->Update();
}

void DungeonScene::Exit()
{
	LOG_DEBUG("Game Scene exited");

	// TODO:保存游戲的進度
	auto cumulativeTime = Util::Time::GetElapsedTimeMs() - m_SceneData->gameProgress.dungeonStartTime;
	m_SceneData->gameProgress.cumulativeTime += cumulativeTime;

	if (m_Player)
	{
		SavePlayerInformation(m_Player);
	}
}

Scene::SceneType DungeonScene::Change()
{
	if (Util::Input::IsKeyUp(Util::Keycode::RETURN))
		return Scene::SceneType::Menu;
	if (!m_Player->IsActive())
	{
		m_timer += Util::Time::GetDeltaTimeMs() / 1000.0f;
		// m_OnDeathText->SetControlVisible(true);

		if (m_timer >= 2.0f)
		{
			m_timer = 0.0f;
			// m_OnDeathText->SetControlVisible(false);
			return Scene::SceneType::DungeonLoad;
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

void DungeonScene::CreatePlayer()
{
	// 使用 CharacterFactory 创建玩家
	m_Player = CharacterFactory::GetInstance().createPlayer(1);
	if (!m_Player) LOG_ERROR("Failed to create player");
	const auto playerData = m_SceneData->gameProgress.playerData;

	// Hp & 能量
	if (const auto healthComp = m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH)){
		const auto hp = playerData.currentHp;
		const auto energy = playerData.currentEnergy;
		healthComp->SetCurrentHp(hp);
		healthComp->SetCurrentEnergy(energy);
	}

	std::vector<Talent> talentDatabase = CreateTalentList(); // 創建天賦資料庫
	if (auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT))
	{
		talentComp->AddTalent(talentDatabase[2]);
	// 武器
	if (const auto attackComp = m_Player->GetComponent<AttackComponent>(ComponentType::ATTACK))
	{
		// 移除初始武器
		attackComp->RemoveAllWeapon();
		auto weaponID = playerData.weaponID;
		for (const auto& id : weaponID){
			attackComp->AddWeapon(WeaponFactory::createWeapon(id));
		}
	}

	// 恢復天賦
	std::vector<Talent> talentDatabase = CreateTalentList();  // 創建天賦資料庫
	if (const auto talentComp = m_Player->GetComponent<TalentComponent>(ComponentType::TALENT)){
		auto talentID = playerData.talentID;
		for (const auto& talent : talentID)
		talentComp->AddTalent(talentDatabase[talent]);
	}

	// money
	// if (const auto XXComp = m_Player->GetComponent<>(ComponentType::)){
	//
	// }

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

void DungeonScene::InitUIManager() {
	UIManager::GetInstance().ResetPanels();

	const auto settingPanel = std::make_shared<SettingPanel>();
	settingPanel->Start();
	UIManager::GetInstance().RegisterPanel("setting", settingPanel);

	const auto playerStatusPanel = std::make_shared<PlayerStatusPanel>(m_Player->GetComponent<HealthComponent>(ComponentType::HEALTH));
	playerStatusPanel->Start();
	UIManager::GetInstance().RegisterPanel("playerStatus", playerStatusPanel);
}

void DungeonScene::InitAudioManager() {
	AudioManager::GetInstance().Reset();
	AudioManager::GetInstance().LoadFromJson("/Lobby/AudioConfig.json");
	AudioManager::GetInstance().PlayBGM();
}