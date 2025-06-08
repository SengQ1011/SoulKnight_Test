//
// Created by QuzzS on 2025/4/23.
//

#include "Room/MonsterRoom.hpp"
#include <algorithm>
#include <random>
#include "Components/AttackComponent.hpp"
#include "Components/DoorComponent.hpp"
#include "Creature/Character.hpp"
#include "Factory/CharacterFactory.hpp"
#include "Loader.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "Tool/Tool.hpp"
#include "Util/Input.hpp"
#include "Util/Keycode.hpp"
#include "Util/Logger.hpp"


#include "ObserveManager/EventManager.hpp"
#include "Structs/DeathEventInfo.hpp"
#include "Structs/EventInfo.hpp"
#include "Weapon/Weapon.hpp"

void MonsterRoom::Start(const std::shared_ptr<Character> &player)
{
	DungeonRoom::Start(player);
	LoadCombatConfiguration();

	// 先初始化戰鬥管理器
	m_CombatManager.Initialize(m_WaveConfigs);

	// 然後預先生成所有波次的怪物
	PreSpawnAllWaveEnemies();

	// 測試門事件系統 - 確保門已經初始化後再測試
	// 這將在房間啟動時測試一次門的開關
}

void MonsterRoom::Update()
{
	DungeonRoom::Update();
	m_CombatManager.Update();
}

void MonsterRoom::LoadFromJSON()
{
	// 使用隨機佈局載入
	const nlohmann::json jsonData = m_Loader.lock()->LoadMonsterRoomObjectPosition_Random();
	InitializeRoomObjects(jsonData);
}

void MonsterRoom::LoadFromJSON_Specific(const std::string &layoutName)
{
	// 使用指定佈局載入（測試用）
	const nlohmann::json jsonData = m_Loader.lock()->LoadMonsterRoomObjectPosition_Specific(layoutName);
	InitializeRoomObjects(jsonData);
}

void MonsterRoom::ChangeLayoutRuntime(const std::string &layoutName)
{
	if (!CanChangeLayout())
	{
		LOG_WARN("Cannot change layout in current room state: {}", static_cast<int>(m_State));
		return;
	}

	LOG_INFO("Changing room layout to: {}", layoutName.empty() ? "random" : layoutName);

	// 1. 保存當前的連接資訊
	std::vector<Direction> connectedDirections = GetConnectedDirections();

	// 2. 清理現有房間物件（保留敵人和玩家）
	ClearNonEssentialObjects();

	// 3. 重新載入新佈局
	if (layoutName.empty() || layoutName == "RANDOM")
	{
		LoadFromJSON(); // 使用隨機佈局
	}
	else
	{
		LoadFromJSON_Specific(layoutName); // 使用指定佈局
	}

	// 4. 重建網格系統
	InitializeGrid();

	// 5. 重建通道和牆壁（基於原有連接資訊）
	RebuildConnectionsAndWalls();

	// 6. 完成房間設置
	FinalizeRoomSetup();

	// 7. 重新定位所有實體，避免卡牆或位置衝突
	RepositionEntitiesAfterLayoutChange();

	LOG_INFO("Room layout changed successfully with {} connections restored", connectedDirections.size());
}

bool MonsterRoom::CanChangeLayout() const
{
	// 只允許在非戰鬥狀態下更換佈局
	return m_State != RoomState::COMBAT;
}

void MonsterRoom::ClearNonEssentialObjects()
{
	// 清理所有房間物件，但保留敵人和玩家
	auto objectsCopy = m_RoomObjects; // 複製避免迭代時修改
	for (auto &obj : objectsCopy)
	{
		// 跳過角色實體（敵人和玩家）
		if (const auto character = std::dynamic_pointer_cast<Character>(obj))
		{
			if (character->GetType() == CharacterType::PLAYER)
				continue;
			RemoveEntity(character);
			continue;
		}

		// 移除其他物件（牆壁、地板、道具等）
		RemoveRoomObject(obj);
	}

	// 清理門列表
	m_Doors.clear();

	LOG_DEBUG("Cleared {} non-essential objects", objectsCopy.size() - m_RoomObjects.size());
}

void MonsterRoom::RebuildConnectionsAndWalls()
{
	// 重建所有方向的通道或牆壁
	const std::array<Direction, 4> allDirections = {Direction::UP, Direction::RIGHT, Direction::DOWN, Direction::LEFT};

	for (Direction dir : allDirections)
	{
		if (HasConnectionToDirection(dir))
		{
			// 有連接的方向創建通道
			CreateCorridorInDirection(dir);
			LOG_DEBUG("Rebuilt corridor in direction: {}", static_cast<int>(dir));
		}
		else
		{
			// 無連接的方向創建牆壁
			CreateWallInDirection(dir);
			LOG_DEBUG("Rebuilt wall in direction: {}", static_cast<int>(dir));
		}
	}
}

void MonsterRoom::TryActivateByPlayer()
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

void MonsterRoom::OnStateChanged()
{
	switch (m_State)
	{
	case RoomState::COMBAT:
		CloseDoors();
		m_CombatManager.StartCombat();
		break;

	case RoomState::EXPLORED:
		OpenDoors();
		SpawnRewardChest();
		break;

	default:
		LOG_DEBUG("MonsterRoom::OnStateChanged: No action needed for state {}", static_cast<int>(m_State));
		break;
	}
}


void MonsterRoom::OnEnemyDeathEvent(const EnemyDeathEvent &event)
{
	// 檢查死亡的敵人是否屬於當前房間
	if (auto deadEnemy = event.m_DeadEnemy.lock())
	{
		if (!HasEntity(deadEnemy))
		{
			// 這個敵人不屬於當前房間，忽略事件
			return;
		}
	}
	else
	{
		// 敵人對象已經被銷毀，無法確定歸屬，忽略事件
		return;
	}

	// 處理敵人死亡事件（新的事件驅動方式）
	// 通知戰鬥管理器
	m_CombatManager.OnEnemyDied();
}

std::shared_ptr<Character> MonsterRoom::SpawnEnemy(int enemyType, glm::vec2 position)
{
	auto enemy = CharacterFactory::GetInstance().createEnemy(enemyType);
	enemy->m_WorldCoord = position;
	enemy->SetActive(false); // 默認未激活，等待波次開始
	SetEnemyVisible(enemy, false);

	// 使用新的Room API
	SpawnEntity(enemy, EntityCategory::ENEMY);

	return enemy;
}

void MonsterRoom::LoadCombatConfiguration()
{
	// 隨機決定波次數量
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> waveDist(0.0f, 1.0f);
	float randomValue = waveDist(rng);
	int totalWaves;
	if (randomValue < 0.7f)
		totalWaves = 1;
	else if (randomValue < 0.95f)
		totalWaves = 2;
	else
		totalWaves = 3;

	m_WaveConfigs.clear();

	// 生成配置（每波次固定3只怪物） TODO: 怪物點數
	for (int wave = 0; wave < totalWaves; ++wave)
	{
		WaveConfig waveConfig;
		waveConfig.enemyCount = 3; // 固定每波次3只怪物

		// 生成3個隨機敵人ID (1-14)
		std::uniform_int_distribution<int> enemyIdDist(1, 14);
		for (int i = 0; i < 3; ++i)
		{
			waveConfig.enemyTypes.push_back(enemyIdDist(rng));
		}

		waveConfig.spawnDelay = 0.5f;
		m_WaveConfigs.push_back(waveConfig);
	}
}

void MonsterRoom::PreSpawnAllWaveEnemies()
{
	// 生成足夠的隨機位置（所有波次的怪物總數）
	int totalEnemies = 0;
	for (const auto &config : m_WaveConfigs)
	{
		totalEnemies += config.enemyCount;
	}

	std::vector<glm::vec2> allPositions = GenerateSpawnPositions(totalEnemies);
	int positionIndex = 0;


	// 為每個波次生成怪物
	for (size_t waveIndex = 0; waveIndex < m_WaveConfigs.size(); ++waveIndex)
	{
		const WaveConfig &config = m_WaveConfigs[waveIndex];
		std::vector<std::shared_ptr<Character>> waveEnemies;

		// 生成當前波次的怪物
		for (int enemyIndex = 0; enemyIndex < config.enemyCount; ++enemyIndex)
		{
			if (positionIndex >= static_cast<int>(allPositions.size()))
			{
				LOG_ERROR("Not enough spawn positions! Needed {}, got {}", totalEnemies, allPositions.size());
				break;
			}

			int enemyType = config.enemyTypes[enemyIndex];
			glm::vec2 position = allPositions[positionIndex++];

			auto enemy = SpawnEnemy(enemyType, position);
			if (enemy)
			{
				waveEnemies.push_back(enemy);
			}
			else
			{
				LOG_ERROR("Failed to spawn enemy type {}", enemyType);
			}

			// 顯示第一波次敵人
			if (waveIndex == 0)
			{
				SetEnemyVisible(enemy, true);
			}
		}

		// 將當前波次的怪物添加到戰鬥管理器
		m_CombatManager.AddWaveEnemies(static_cast<int>(waveIndex), waveEnemies);
	}
}

std::vector<glm::vec2> MonsterRoom::GenerateSpawnPositions(int count)
{
	std::vector<glm::vec2> positions;
	const glm::vec2 &tileSize = m_RoomSpaceInfo.m_TileSize;
	const glm::vec2 &roomSize = m_RoomSpaceInfo.m_RoomSize;
	const glm::vec2 &region = m_RoomSpaceInfo.m_RoomRegion;
	const glm::vec2 &roomCoord = m_RoomSpaceInfo.m_WorldCoord;

	const int regionWidth = static_cast<int>(region.x);
	const int regionHeight = static_cast<int>(region.y);
	const int roomWidth = static_cast<int>(roomSize.x);
	const int roomHeight = static_cast<int>(roomSize.y);

	// 修正：計算房間內部可生成的區域（避免邊界）
	const int centerX = regionWidth / 2;
	const int centerY = regionHeight / 2;
	const int startX = centerX - roomWidth / 2 + 1; // 避開邊界牆壁
	const int startY = centerY - roomHeight / 2 + 1;
	const int endX = centerX + roomWidth / 2 - 1;
	const int endY = centerY + roomHeight / 2 - 1;

	// 確保範圍有效
	if (startX >= endX || startY >= endY)
	{
		LOG_ERROR("Invalid spawn area: startX={}, endX={}, startY={}, endY={}", startX, endX, startY, endY);
		return positions;
	}

	// 創建可用位置列表（使用新的網格系統）
	std::vector<glm::ivec2> availablePositions;
	for (int row = startY; row < endY; ++row)
	{
		for (int col = startX; col < endX; ++col)
		{
			// 使用新的網格系統檢查位置是否被阻擋
			if (!IsGridPositionBlocked(row, col))
			{
				availablePositions.push_back({col, row});
			}
		}
	}

	// 檢查是否有足夠的可用位置
	if (availablePositions.size() < static_cast<size_t>(count))
	{
		LOG_ERROR("Not enough available positions! Requested: {}, Available: {}", count, availablePositions.size());
		// 返回所有可用位置，而不是失敗
		count = static_cast<int>(availablePositions.size());
	}

	// 隨機選擇位置
	std::mt19937 rng(std::random_device{}());
	std::shuffle(availablePositions.begin(), availablePositions.end(), rng);

	// 轉換為世界座標
	for (int i = 0; i < count && i < static_cast<int>(availablePositions.size()); ++i)
	{
		const glm::ivec2 &gridPos = availablePositions[i];
		glm::vec2 worldPos = Tool::RoomGridToWorld(gridPos, tileSize, roomCoord, region);
		positions.push_back(worldPos);
	}

	return positions;
}

void MonsterRoom::CloseDoors()
{
	if (m_Doors.empty())
	{
		LOG_ERROR("MonsterRoom::CloseDoors: No door in the room");
		return;
	}

	// 使用事件系統通知所有門關閉
	DoorCloseEvent closeEvent;
	EventManager::GetInstance().Emit(closeEvent);
}

void MonsterRoom::OpenDoors()
{
	// 使用事件系統通知所有門開啟
	DoorOpenEvent openEvent;
	EventManager::GetInstance().Emit(openEvent);
}

void MonsterRoom::SpawnRewardChest()
{
	if (const auto rewardChest = CreateChest(ChestType::REWARD))
	{
		rewardChest->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
	}
}

MonsterRoom::CombatManager::CombatManager(MonsterRoom *room) : m_Room(room) {}

void MonsterRoom::CombatManager::Initialize(const std::vector<WaveConfig> &waveConfigs)
{
	m_WaveConfigs = waveConfigs;
	m_CombatState = CombatState::INACTIVE;
	m_CurrentWave = 0;

	// 初始化所有波次的敵人容器
	m_AllWaveEnemies.clear();
	m_AllWaveEnemies.resize(waveConfigs.size());
}

void MonsterRoom::CombatManager::AddWaveEnemies(int waveIndex, const std::vector<std::shared_ptr<Character>> &enemies)
{
	if (waveIndex >= 0 && waveIndex < static_cast<int>(m_AllWaveEnemies.size()))
	{
		m_AllWaveEnemies[waveIndex].clear();
		for (const auto &enemy : enemies)
		{
			m_AllWaveEnemies[waveIndex].push_back(enemy);
		}
	}
}

void MonsterRoom::CombatManager::Update()
{
	// 清理已死亡的敵人引用
	CleanupDeadEnemies();

	// 檢查波次完成狀態
	if (m_CombatState == CombatState::WAVE_ACTIVE)
	{
		CheckWaveCompletion();
	}
}

void MonsterRoom::CombatManager::StartCombat()
{
	if (m_WaveConfigs.empty())
	{
		LOG_ERROR("CombatManager::StartCombat: No wave configurations available");
		return;
	}

	m_CombatState = CombatState::WAVE_ACTIVE;
	m_CurrentWave = 0;
	StartCurrentWave();
}

void MonsterRoom::CombatManager::EndCombat() { m_CombatState = CombatState::COMPLETED; }

void MonsterRoom::CombatManager::StartCurrentWave()
{
	if (m_CurrentWave >= static_cast<int>(m_WaveConfigs.size()))
	{
		EndCombat();
		return;
	}

	m_CombatState = CombatState::WAVE_ACTIVE;

	// 激活當前波次的怪物（不需要生成，因為已經預先生成了）
	ActivateCurrentWaveEnemies();
}

void MonsterRoom::CombatManager::ActivateCurrentWaveEnemies()
{
	if (m_CurrentWave >= static_cast<int>(m_AllWaveEnemies.size()))
	{
		LOG_ERROR("CurrentWave {} >= AllWaveEnemies size {}", m_CurrentWave, m_AllWaveEnemies.size());
		return;
	}

	// 先隱藏所有波次的敵人
	// HideAllEnemies();

	const auto &currentWaveEnemies = m_AllWaveEnemies[m_CurrentWave];

	// 只激活和顯示當前波次的敵人
	for (const auto &weakEnemy : currentWaveEnemies)
	{
		if (auto enemy = weakEnemy.lock())
		{
			enemy->SetActive(true); // 激活怪物
			SetEnemyVisible(enemy, true);
		}
	}
}

void MonsterRoom::CombatManager::OnEnemyDied()
{
	// Update方法中的CheckWaveCompletion會處理波次完成檢查
}

int MonsterRoom::CombatManager::GetAliveEnemiesInCurrentWave() const
{
	if (m_CurrentWave >= static_cast<int>(m_AllWaveEnemies.size()))
		return 0;

	const auto &currentWaveEnemies = m_AllWaveEnemies[m_CurrentWave];
	int count = 0;

	for (const auto &weakEnemy : currentWaveEnemies)
	{
		if (auto enemy = weakEnemy.lock())
		{
			if (enemy->IsActive()) // 活著且激活的敵人
				count++;
		}
	}
	return count;
}

void MonsterRoom::CombatManager::CheckWaveCompletion()
{
	if (GetAliveEnemiesInCurrentWave() == 0)
	{
		m_CombatState = CombatState::WAVE_CLEARING;

		if (m_CurrentWave + 1 < static_cast<int>(m_WaveConfigs.size()))
		{
			StartNextWave();
		}
		else
		{
			EndCombat();
		}
	}
}

void MonsterRoom::CombatManager::StartNextWave()
{
	m_CurrentWave++;
	StartCurrentWave();
}

void MonsterRoom::CombatManager::CleanupDeadEnemies()
{
	// 清理所有波次中已失效的弱指針
	for (auto &waveEnemies : m_AllWaveEnemies)
	{
		waveEnemies.erase(std::remove_if(waveEnemies.begin(), waveEnemies.end(),
										 [](const std::weak_ptr<Character> &weakPtr) { return weakPtr.expired(); }),
						  waveEnemies.end());
	}
}

void MonsterRoom::CombatManager::HideAllEnemies()
{
	// 隱藏所有波次的敵人
	for (const auto &waveEnemies : m_AllWaveEnemies)
	{
		for (const auto &weakEnemy : waveEnemies)
		{
			if (auto enemy = weakEnemy.lock())
			{
				enemy->SetActive(false); // 設為非激活
				SetEnemyVisible(enemy, false);
			}
		}
	}
}

void MonsterRoom::SetEnemyVisible(const std::shared_ptr<Character> &enemy, bool visible)
{
	if (!enemy)
		return;

	// 控制敵人本身的可見性
	enemy->SetControlVisible(visible);

	// 控制敵人武器的可見性
	if (auto attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK))
	{
		if (auto weapon = attackComp->GetCurrentWeapon())
		{
			weapon->SetControlVisible(visible);
		}
	}
}

void MonsterRoom::CombatManager::SetEnemyVisible(const std::shared_ptr<Character> &enemy, bool visible)
{
	if (!enemy)
		return;

	// 控制敵人本身的可見性
	enemy->SetControlVisible(visible);

	// 控制敵人武器的可見性
	if (auto attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK))
	{
		if (auto weapon = attackComp->GetCurrentWeapon())
		{
			weapon->SetControlVisible(visible);
		}
	}
}

void MonsterRoom::RepositionEntitiesAfterLayoutChange()
{
	LOG_DEBUG("Starting entity repositioning after layout change");

	// 重新定位怪物
	RepositionEnemies();

	// 重新定位玩家（如果需要）
	RepositionPlayerIfNeeded();

	LOG_DEBUG("Entity repositioning completed");
}

void MonsterRoom::RepositionEnemies()
{
	auto enemies = GetEnemies();
	if (enemies.empty())
	{
		LOG_DEBUG("No enemies to reposition");
		return;
	}

	LOG_DEBUG("Repositioning {} enemies", enemies.size());

	// 生成新的安全位置
	std::vector<glm::vec2> safePositions = GenerateSpawnPositions(static_cast<int>(enemies.size()));

	if (safePositions.size() < enemies.size())
	{
		LOG_WARN("Not enough safe positions for all enemies. Expected {}, got {}", enemies.size(),
				 safePositions.size());
	}

	// 重新定位每個怪物
	for (size_t i = 0; i < enemies.size() && i < safePositions.size(); ++i)
	{
		auto enemy = enemies[i];
		glm::vec2 oldPos = enemy->m_WorldCoord;
		enemy->m_WorldCoord = safePositions[i];

		LOG_DEBUG("Moved enemy from ({:.1f}, {:.1f}) to ({:.1f}, {:.1f})", oldPos.x, oldPos.y, safePositions[i].x,
				  safePositions[i].y);
	}
}

void MonsterRoom::RepositionPlayerIfNeeded()
{
	auto player = m_Player.lock();
	if (!player)
	{
		LOG_DEBUG("No player to reposition");
		return;
	}

	// 檢查玩家是否在房間內部
	if (!IsPlayerInsideRoom())
	{
		LOG_DEBUG("Player is not inside room, no repositioning needed");
		return;
	}

	LOG_DEBUG("Player is inside room, moving player outside");

	const glm::vec2 &roomCenter = m_RoomSpaceInfo.m_WorldCoord;
	const glm::vec2 &roomSize = m_RoomSpaceInfo.m_RoomSize;
	const glm::vec2 &tileSize = m_RoomSpaceInfo.m_TileSize;
	const glm::vec2 playerPos = player->m_WorldCoord;

	// 計算玩家相對於房間中心的方向向量
	glm::vec2 direction = playerPos - roomCenter;

	// 如果玩家正好在房間中心，隨便選一個方向（向左）
	if (glm::length(direction) < 1.0f)
	{
		direction = glm::vec2(-1.0f, 0.0f);
	}
	else
	{
		direction = glm::normalize(direction);
	}

	// 計算房間內範圍的一半長度（加上一點緩衝）
	const float roomHalfSize = (roomSize.x * tileSize.x) / 2.0f;
	const float safeDistance = roomHalfSize + 2.0f * tileSize.x; // 房間邊緣外2格

	// 計算新位置：房間中心 + 方向 * 安全距離
	glm::vec2 newPosition = roomCenter + direction * safeDistance;

	glm::vec2 oldPos = player->m_WorldCoord;
	player->m_WorldCoord = newPosition;

	LOG_INFO("Moved player from ({:.1f}, {:.1f}) to ({:.1f}, {:.1f}) - outside room", oldPos.x, oldPos.y, newPosition.x,
			 newPosition.y);
}
