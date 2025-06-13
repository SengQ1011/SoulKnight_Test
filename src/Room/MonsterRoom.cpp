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
#include "imgui.h"


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

	// LOG_DEBUG("Cleared {} non-essential objects", objectsCopy.size() - m_RoomObjects.size());
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

void MonsterRoom::AddEnemy(const std::shared_ptr<Character> &enemy)
{
	if (!enemy)
	{
		LOG_ERROR("MonsterRoom::AddEnemy: Enemy is null");
		return;
	}

	enemy->SetActive(true);
	SetEnemyVisible(enemy, true);

	// 使用新的Room API添加敌人
	SpawnEntity(enemy, EntityCategory::ENEMY);

	m_CombatManager.AddEnemyToCurrentWave(enemy);
}

std::shared_ptr<Character> MonsterRoom::SpawnEnemy(int enemyType, glm::vec2 position)
{
	auto enemy = CharacterFactory::GetInstance().createEnemy(enemyType);
	// auto enemy = CharacterFactory::GetInstance().createEnemy(101);
	enemy->m_WorldCoord = position;
	HideEvent hideEvent;
	enemy->OnEvent(hideEvent);
	enemy->SetActive(false); // 默認未激活，等待波次開始

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

void MonsterRoom::RecalculateGridFromCollisionComponents()
{
	LOG_DEBUG("Starting grid recalculation from CollisionComponents");

	// 1. 清空當前網格
	m_GridSystem->Initialize(); // 重置網格為全空狀態

	// 2. 重新標記牆壁和門（這些是地形，需要保留）
	const glm::vec2 &region = m_RoomSpaceInfo.m_RoomRegion;
	const glm::vec2 &roomSize = m_RoomSpaceInfo.m_RoomSize;
	const int regionWidth = static_cast<int>(region.x);
	const int regionHeight = static_cast<int>(region.y);
	const int roomWidth = static_cast<int>(roomSize.x);
	const int roomHeight = static_cast<int>(roomSize.y);
	const int centerX = regionWidth / 2;
	const int centerY = regionHeight / 2;

	// 標記房間邊界牆壁
	const int roomStartX = centerX - roomWidth / 2;
	const int roomEndX = centerX + roomWidth / 2;
	const int roomStartY = centerY - roomHeight / 2;
	const int roomEndY = centerY + roomHeight / 2;

	// 標記房間邊界為阻擋
	for (int row = roomStartY; row <= roomEndY; ++row)
	{
		for (int col = roomStartX; col <= roomEndX; ++col)
		{
			// 邊界牆壁
			if (row == roomStartY || row == roomEndY || col == roomStartX || col == roomEndX)
			{
				m_GridSystem->MarkPosition(row, col, 1);
			}
		}
	}

	// 3. 遍歷所有房間物件，重新計算具有 CollisionComponent 的物件
	int processedObjects = 0;
	int collisionObjects = 0;

	for (const auto &obj : m_RoomObjects)
	{
		if (!obj)
			continue;

		processedObjects++;

		// 檢查是否有 CollisionComponent
		auto collisionComp = obj->GetComponent<CollisionComponent>(ComponentType::COLLISION);
		if (!collisionComp)
		{
			continue;
		}

		collisionObjects++;

		// 獲取物件的碰撞邊界
		Rect bounds = collisionComp->GetBounds();
		glm::vec2 objSize = collisionComp->GetSize();

		LOG_DEBUG(
			"Processing object with CollisionComponent: Size=({:.1f}, {:.1f}), Bounds=[{:.1f}, {:.1f}, {:.1f}, {:.1f}]",
			objSize.x, objSize.y, bounds.left(), bounds.top(), bounds.right(), bounds.bottom());

		// 使用與 GridSystem::UpdateGridFromObjects 相同的邏輯
		const float gridSize = m_RoomSpaceInfo.m_RoomRegion.x;
		const float tileSize = m_RoomSpaceInfo.m_TileSize.x;

		// 計算左上角格子世界坐標軸
		auto startPos = glm::vec2(gridSize * tileSize / 2) - glm::vec2(tileSize / 2);
		startPos *= glm::vec2(-1, 1);
		startPos += m_RoomSpaceInfo.m_WorldCoord;

		// 計算物件佔據的網格範圍
		glm::ivec2 startGrid = glm::vec2(std::floor((bounds.left() - startPos.x) / tileSize),
										 std::ceil((startPos.y - bounds.top()) / tileSize));
		glm::ivec2 endGrid = glm::vec2(std::ceil((bounds.right() - startPos.x) / tileSize),
									   std::floor((startPos.y - bounds.bottom()) / tileSize));

		// 標記佔據的網格
		int markedCells = 0;
		for (int row = startGrid.y; row <= endGrid.y; row++)
		{
			for (int col = startGrid.x; col <= endGrid.x; col++)
			{
				// 檢查網格是否在有效範圍內
				if (row >= 0 && row < RoomConstants::GRID_SIZE && col >= 0 && col < RoomConstants::GRID_SIZE)
				{
					// 計算網格中心位置
					glm::vec2 posGridCell =
						startPos + glm::vec2(static_cast<float>(col) * tileSize, -static_cast<float>(row) * tileSize);
					glm::vec2 sizeGridCell(tileSize);
					Rect gridCell(posGridCell, sizeGridCell);

					// 計算交集面積
					float intersect = CalculateIntersectionArea(bounds, gridCell);
					float cellArea = tileSize * tileSize;

					// 如果交集面積超過閾值，標記為佔據
					if (intersect >= RoomConstants::INTERSECTION_THRESHOLD * cellArea)
					{
						m_GridSystem->MarkPosition(row, col, 1);
						markedCells++;
					}
				}
			}
		}

		// LOG_DEBUG("Object marked {} grid cells", markedCells);
	}

	// LOG_DEBUG("Grid recalculation complete: Processed {} objects, {} with CollisionComponent", processedObjects,
	// collisionObjects);
}

// 輔助方法：計算兩個矩形的交集面積
float MonsterRoom::CalculateIntersectionArea(const Rect &a, const Rect &b) const
{
	const float left = std::max(a.left(), b.left());
	const float right = std::min(a.right(), b.right());
	const float top = std::max(a.top(), b.top());
	const float bottom = std::min(a.bottom(), b.bottom());

	if (right <= left || top <= bottom)
		return 0.0f;

	return (right - left) * (top - bottom);
}

void MonsterRoom::PreSpawnAllWaveEnemies()
{
	// ===== 重新計算所有碰撞物件佔據的格子 =====
	RecalculateGridFromCollisionComponents();

	// 改進的算法：為每個怪物單獨生成適合其體型的位置
	std::vector<glm::ivec2> occupiedPositions; // 追蹤已佔用的網格位置

	// ===== 調試：輸出當前網格狀態 =====
	LOG_DEBUG("=== PreSpawnAllWaveEnemies Debug Info ===");
	LOG_DEBUG("Room World Coord: ({:.1f}, {:.1f})", m_RoomSpaceInfo.m_WorldCoord.x, m_RoomSpaceInfo.m_WorldCoord.y);
	LOG_DEBUG("Room Size: ({:.1f}, {:.1f})", m_RoomSpaceInfo.m_RoomSize.x, m_RoomSpaceInfo.m_RoomSize.y);
	LOG_DEBUG("Tile Size: ({:.1f}, {:.1f})", m_RoomSpaceInfo.m_TileSize.x, m_RoomSpaceInfo.m_TileSize.y);

	// 統計被阻擋的網格數量
	const auto &gridData = GetGridData();
	int blockedCount = 0;
	int totalCount = gridData.size() * gridData[0].size();
	for (const auto &row : gridData)
	{
		for (int cell : row)
		{
			if (cell != 0)
				blockedCount++;
		}
	}
	LOG_DEBUG("Grid Status: {}/{} positions blocked ({:.1f}%)", blockedCount, totalCount,
			  (float)blockedCount / totalCount * 100.0f);

	// 為每個波次生成怪物
	for (size_t waveIndex = 0; waveIndex < m_WaveConfigs.size(); ++waveIndex)
	{
		const WaveConfig &config = m_WaveConfigs[waveIndex];
		std::vector<std::shared_ptr<Character>> waveEnemies;

		LOG_DEBUG("=== Wave {} Generation ===", waveIndex + 1);

		// 生成當前波次的怪物
		for (int enemyIndex = 0; enemyIndex < config.enemyCount; ++enemyIndex)
		{
			int enemyType = config.enemyTypes[enemyIndex];

			// 首先創建怪物以獲取其體型信息
			auto tempEnemy = CharacterFactory::GetInstance().createEnemy(enemyType);
			if (!tempEnemy)
			{
				LOG_ERROR("Failed to create enemy type {} for size checking", enemyType);
				continue;
			}

			// 獲取怪物的實際體型
			glm::vec2 enemySize(16.0f, 16.0f); // 預設大小
			if (auto collisionComp = tempEnemy->GetComponent<CollisionComponent>(ComponentType::COLLISION))
			{
				enemySize = collisionComp->GetSize();
			}

			LOG_DEBUG("Enemy {} (Type {}): Size = ({:.1f}, {:.1f})", enemyIndex + 1, enemyType, enemySize.x,
					  enemySize.y);

			// 生成適合該體型的位置（避開已佔用位置）
			auto availablePositions = GenerateSpawnPositionsWithSizeAndExclusions(1, enemySize, occupiedPositions);

			if (availablePositions.empty())
			{
				LOG_WARN("No available position for enemy type {} with size {}x{}", enemyType, enemySize.x,
						 enemySize.y);

				// ===== 詳細調試：檢查為什麼沒有可用位置 =====
				LOG_DEBUG("Debug: Checking available positions for enemy type {}", enemyType);
				const glm::vec2 &tileSize = m_RoomSpaceInfo.m_TileSize;
				const glm::vec2 &roomSize = m_RoomSpaceInfo.m_RoomSize;
				const glm::vec2 &region = m_RoomSpaceInfo.m_RoomRegion;

				const int regionWidth = static_cast<int>(region.x);
				const int regionHeight = static_cast<int>(region.y);
				const int roomWidth = static_cast<int>(roomSize.x);
				const int roomHeight = static_cast<int>(roomSize.y);

				const int entityGridWidth = static_cast<int>(std::ceil(enemySize.x / tileSize.x));
				const int entityGridHeight = static_cast<int>(std::ceil(enemySize.y / tileSize.y));

				const int centerX = regionWidth / 2;
				const int centerY = regionHeight / 2;
				const int startX = centerX - roomWidth / 2 + 1;
				const int startY = centerY - roomHeight / 2 + 1;
				const int endX = centerX + roomWidth / 2 - 1 - (entityGridWidth - 1);
				const int endY = centerY + roomHeight / 2 - 1 - (entityGridHeight - 1);

				LOG_DEBUG("Search area: X=[{}, {}), Y=[{}, {})", startX, endX, startY, endY);
				LOG_DEBUG("Entity grid size: {}x{}", entityGridWidth, entityGridHeight);

				// 檢查前幾個位置為什麼被阻擋
				int checkedPositions = 0;
				for (int row = startY; row < endY && checkedPositions < 5; ++row)
				{
					for (int col = startX; col < endX && checkedPositions < 5; ++col)
					{
						checkedPositions++;
						bool isClear = IsAreaClearForEntity({col, row}, entityGridWidth, entityGridHeight);
						LOG_DEBUG("Position ({}, {}) -> {}", col, row, isClear ? "CLEAR" : "BLOCKED");

						if (!isClear)
						{
							// 詳細檢查哪個網格被阻擋
							for (int r = row; r < row + entityGridHeight; ++r)
							{
								for (int c = col; c < col + entityGridWidth; ++c)
								{
									bool blocked = IsGridPositionBlocked(r, c);
									if (blocked)
									{
										LOG_DEBUG("  -> Grid ({}, {}) is BLOCKED", c, r);
									}
								}
							}
						}
					}
				}

				continue;
			}

			glm::vec2 position = availablePositions[0];
			LOG_DEBUG("Enemy {} positioned at ({:.1f}, {:.1f})", enemyIndex + 1, position.x, position.y);

			// 記錄此怪物佔用的網格位置
			RecordOccupiedPositions(position, enemySize, occupiedPositions);

			// 設置怪物位置並添加到場景
			auto enemy = SpawnEnemy(enemyType, position);
			if (enemy)
			{
				waveEnemies.push_back(enemy);

				if (waveIndex == 0)
				{
					enemy->SetActive(true);
					ShowUpEvent showUpEvent;
					enemy->OnEvent(showUpEvent);
					enemy->SetActive(false);
				}
			}
			else
			{
				LOG_ERROR("Failed to spawn enemy type {}", enemyType);
			}
		}

		// 將當前波次的怪物添加到戰鬥管理器
		m_CombatManager.AddWaveEnemies(static_cast<int>(waveIndex), waveEnemies);
		LOG_DEBUG("Wave {} complete: {} enemies spawned", waveIndex + 1, waveEnemies.size());
	}

	LOG_DEBUG("=== PreSpawnAllWaveEnemies Complete ===");
}

std::vector<glm::vec2> MonsterRoom::GenerateSpawnPositions(int count)
{
	// 使用預設體型進行向後兼容
	return GenerateSpawnPositionsWithSize(count, glm::vec2(16.0f, 16.0f));
}

std::vector<glm::vec2> MonsterRoom::GenerateSpawnPositionsWithSize(int count, const glm::vec2 &entitySize)
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

	// 計算實體需要佔用的網格大小
	const int entityGridWidth = static_cast<int>(std::ceil(entitySize.x / tileSize.x));
	const int entityGridHeight = static_cast<int>(std::ceil(entitySize.y / tileSize.y));

	// 修正：計算房間內部可生成的區域（避免邊界，並考慮實體大小）
	const int centerX = regionWidth / 2;
	const int centerY = regionHeight / 2;
	const int startX = centerX - roomWidth / 2 + 1; // 避開邊界牆壁
	const int startY = centerY - roomHeight / 2 + 1;
	const int endX = centerX + roomWidth / 2 - 1 - (entityGridWidth - 1); // 減去實體寬度
	const int endY = centerY + roomHeight / 2 - 1 - (entityGridHeight - 1); // 減去實體高度

	// 確保範圍有效
	if (startX >= endX || startY >= endY)
	{
		LOG_ERROR("Invalid spawn area for entity size {}x{}: startX={}, endX={}, startY={}, endY={}", entitySize.x,
				  entitySize.y, startX, endX, startY, endY);
		return positions;
	}

	// 創建可用位置列表（使用體型感知的檢查）
	std::vector<glm::ivec2> availablePositions;
	for (int row = startY; row < endY; ++row)
	{
		for (int col = startX; col < endX; ++col)
		{
			// 檢查實體佔用的所有網格是否都可用
			if (IsAreaClearForEntity({col, row}, entityGridWidth, entityGridHeight))
			{
				availablePositions.push_back({col, row});
			}
		}
	}

	// 檢查是否有足夠的可用位置
	if (availablePositions.size() < static_cast<size_t>(count))
	{
		LOG_WARN("Not enough available positions for entity size {}x{}! Requested: {}, Available: {}", entitySize.x,
				 entitySize.y, count, availablePositions.size());
		// 返回所有可用位置，而不是失敗
		count = static_cast<int>(availablePositions.size());
	}

	// 隨機選擇位置
	std::mt19937 rng(std::random_device{}());
	std::shuffle(availablePositions.begin(), availablePositions.end(), rng);

	// 轉換為世界座標（格子左上角 → 實體中心）
	for (int i = 0; i < count && i < static_cast<int>(availablePositions.size()); ++i)
	{
		const glm::ivec2 &gridPos = availablePositions[i];

		// 計算實體中心的正確位置
		glm::vec2 entityCenter = CalculateEntityCenterPosition(gridPos, entitySize, entityGridWidth, entityGridHeight,
															   tileSize, roomCoord, region);

		positions.push_back(entityCenter);
	}

	return positions;
}

bool MonsterRoom::IsAreaClearForEntity(const glm::ivec2 &gridPos, int entityGridWidth, int entityGridHeight) const
{
	// 檢查實體佔用的所有網格是否都可用
	for (int row = gridPos.y; row < gridPos.y + entityGridHeight; ++row)
	{
		for (int col = gridPos.x; col < gridPos.x + entityGridWidth; ++col)
		{
			// 檢查網格是否超出邊界或被阻擋
			if (IsGridPositionBlocked(row, col))
			{
				return false;
			}
		}
	}
	return true;
}

std::vector<glm::vec2>
MonsterRoom::GenerateSpawnPositionsWithSizeAndExclusions(int count, const glm::vec2 &entitySize,
														 const std::vector<glm::ivec2> &occupiedPositions)
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

	// 計算實體需要佔用的網格大小
	const int entityGridWidth = static_cast<int>(std::ceil(entitySize.x / tileSize.x));
	const int entityGridHeight = static_cast<int>(std::ceil(entitySize.y / tileSize.y));

	// 計算可生成區域
	const int centerX = regionWidth / 2;
	const int centerY = regionHeight / 2;
	const int startX = centerX - roomWidth / 2 + 1;
	const int startY = centerY - roomHeight / 2 + 1;
	const int endX = centerX + roomWidth / 2 - 1 - (entityGridWidth - 1);
	const int endY = centerY + roomHeight / 2 - 1 - (entityGridHeight - 1);

	if (startX >= endX || startY >= endY)
	{
		LOG_WARN("Invalid spawn area for entity size {}x{}", entitySize.x, entitySize.y);
		return positions;
	}

	// 創建可用位置列表
	std::vector<glm::ivec2> availablePositions;
	for (int row = startY; row < endY; ++row)
	{
		for (int col = startX; col < endX; ++col)
		{
			// 檢查是否與已佔用位置重疊
			if (IsAreaOverlapWithOccupied({col, row}, entityGridWidth, entityGridHeight, occupiedPositions))
			{
				continue;
			}

			// 檢查實體佔用的所有網格是否都可用
			if (IsAreaClearForEntity({col, row}, entityGridWidth, entityGridHeight))
			{
				availablePositions.push_back({col, row});
			}
		}
	}

	// 隨機選擇位置
	std::mt19937 rng(std::random_device{}());
	std::shuffle(availablePositions.begin(), availablePositions.end(), rng);

	// 轉換為世界座標
	int actualCount = std::min(count, static_cast<int>(availablePositions.size()));
	for (int i = 0; i < actualCount; ++i)
	{
		const glm::ivec2 &gridPos = availablePositions[i];

		// 計算實體中心的正確位置
		glm::vec2 entityCenter = CalculateEntityCenterPosition(gridPos, entitySize, entityGridWidth, entityGridHeight,
															   tileSize, roomCoord, region);

		positions.push_back(entityCenter);
	}

	return positions;
}

bool MonsterRoom::IsAreaOverlapWithOccupied(const glm::ivec2 &gridPos, int entityGridWidth, int entityGridHeight,
											const std::vector<glm::ivec2> &occupiedPositions) const
{
	// 檢查新實體的佔用區域是否與已佔用位置重疊
	for (const auto &occupied : occupiedPositions)
	{
		// 簡單矩形重疊檢測
		if (gridPos.x < occupied.x + 1 && gridPos.x + entityGridWidth > occupied.x && gridPos.y < occupied.y + 1 &&
			gridPos.y + entityGridHeight > occupied.y)
		{
			return true; // 有重疊
		}
	}
	return false; // 無重疊
}

void MonsterRoom::RecordOccupiedPositions(const glm::vec2 &worldPos, const glm::vec2 &entitySize,
										  std::vector<glm::ivec2> &occupiedPositions)
{
	const glm::vec2 &tileSize = m_RoomSpaceInfo.m_TileSize;
	const glm::vec2 &roomCoord = m_RoomSpaceInfo.m_WorldCoord;
	const glm::vec2 &region = m_RoomSpaceInfo.m_RoomRegion;

	// 計算實體佔用的網格大小
	const int entityGridWidth = static_cast<int>(std::ceil(entitySize.x / tileSize.x));
	const int entityGridHeight = static_cast<int>(std::ceil(entitySize.y / tileSize.y));

	// 計算實體左上角對應的網格位置
	glm::vec2 entityTopLeft = worldPos - glm::vec2(entitySize.x / 2.0f, -entitySize.y / 2.0f);
	glm::ivec2 startGrid = Tool::WorldToRoomGrid(entityTopLeft, tileSize, roomCoord, region);

	// 記錄所有佔用的網格位置
	for (int row = startGrid.y; row < startGrid.y + entityGridHeight; ++row)
	{
		for (int col = startGrid.x; col < startGrid.x + entityGridWidth; ++col)
		{
			occupiedPositions.push_back({col, row});
		}
	}
}

glm::vec2 MonsterRoom::CalculateEntityCenterPosition(const glm::ivec2 &gridPos, const glm::vec2 &entitySize,
													 int entityGridWidth, int entityGridHeight,
													 const glm::vec2 &tileSize, const glm::vec2 &roomCoord,
													 const glm::vec2 &region) const
{
	// 根據碰撞優化文檔的正確邏輯：
	// 1. 奇數格子實體：放在格子中心
	// 2. 偶數格子實體：放在格子之間

	// 獲取左上角格子的左上角世界座標
	glm::vec2 gridTopLeft = Tool::RoomGridToWorld(gridPos, tileSize, roomCoord, region);

	// 計算實體佔用區域的中心偏移
	// 對於 width×height 的矩形區域，從左上角格子到中心的偏移
	glm::vec2 centerOffset = {(entityGridWidth - 1) * tileSize.x / 2.0f, -(entityGridHeight - 1) * tileSize.y / 2.0f};

	// 格子左上角 → 格子中心座標
	glm::vec2 firstCellCenter = gridTopLeft + glm::vec2(tileSize.x / 2.0f, -tileSize.y / 2.0f);

	// 實體中心 = 第一個格子中心 + 中心偏移
	glm::vec2 entityCenter = firstCellCenter + centerOffset;

	return entityCenter;
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
		// 使用與怪物生成相同的隨機位置邏輯
		std::vector<glm::vec2> availablePositions = GenerateSpawnPositions(1);

		if (!availablePositions.empty())
		{
			// 設置寶箱位置為隨機空地
			rewardChest->SetWorldCoord(availablePositions[0]);
		}
		else
		{
			// 如果沒有可用位置，則放在房間中心作為備選方案
			LOG_WARN("No available positions for reward chest, placing at room center");
			rewardChest->SetWorldCoord(m_RoomSpaceInfo.m_WorldCoord);
		}
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

	DrawDebugUI();

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

	const auto &currentWaveEnemies = m_AllWaveEnemies[m_CurrentWave];

	// 激活當前波次的敵人並通過ShowUpEvent顯示
	for (const auto &weakEnemy : currentWaveEnemies)
	{
		if (auto enemy = weakEnemy.lock())
		{
			enemy->SetActive(true); // 激活怪物

			// 使用ShowUpEvent來顯示怪物
			ShowUpEvent showUpEvent;
			enemy->OnEvent(showUpEvent);
		}
	}
}

void MonsterRoom::CombatManager::AddEnemyToCurrentWave(const std::shared_ptr<Character> &enemy)
{
	if (m_CurrentWave >= 0 && m_CurrentWave < static_cast<int>(m_AllWaveEnemies.size()))
	{
		m_AllWaveEnemies[m_CurrentWave].push_back(enemy);
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
				SetEnemyVisible(enemy, false); // 先觸發HideEvent
				enemy->SetActive(false); // 然後設為非激活
			}
		}
	}
}

void MonsterRoom::SetEnemyVisible(const std::shared_ptr<Character> &enemy, bool visible)
{
	if (!enemy)
		return;

	if (visible)
	{
		// 使用ShowUpEvent來顯示敵人和武器
		ShowUpEvent showUpEvent;
		enemy->OnEvent(showUpEvent);
	}
	else
	{
		// 使用HideEvent來隱藏敵人和武器
		HideEvent hideEvent;
		enemy->OnEvent(hideEvent);
	}
}

void MonsterRoom::CombatManager::SetEnemyVisible(const std::shared_ptr<Character> &enemy, bool visible)
{
	if (!enemy)
		return;

	if (visible)
	{
		// 使用ShowUpEvent來顯示敵人和武器
		ShowUpEvent showUpEvent;
		enemy->OnEvent(showUpEvent);
	}
	else
	{
		// 使用HideEvent來隱藏敵人和武器
		HideEvent hideEvent;
		enemy->OnEvent(hideEvent);
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

// ===== MonsterRoom Debug UI 實現 =====
void MonsterRoom::DrawDebugUI()
{
	// 這個方法現在被整合到 Dungeon_Scene 的統一 debug 窗口中
	// 如果需要獨立的 debug 窗口，可以取消註釋以下代碼

	/*
	ImGui::Begin("MonsterRoom Debug UI");

	// === 房間基本信息 ===
	if (ImGui::CollapsingHeader("Room Info", ImGuiTreeNodeFlags_DefaultOpen))
	{
		const char *stateNames[] = {"UNEXPLORED", "COMBAT", "EXPLORED"};
		ImGui::Text("Room State: %s", stateNames[static_cast<int>(m_State)]);
		ImGui::Text("Grid Position: (%.0f, %.0f)", m_MapGridPos.x, m_MapGridPos.y);
		ImGui::Text("World Coord: (%.1f, %.1f)", m_RoomSpaceInfo.m_WorldCoord.x, m_RoomSpaceInfo.m_WorldCoord.y);
	}

	// === 戰鬥管理器 Debug UI ===
	m_CombatManager.DrawDebugUI();

	// === 房間操作 ===
	if (ImGui::CollapsingHeader("Room Operations", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// 佈局更換功能
		ImGui::Text("Layout Change:");
		if (ImGui::Button("Change to Random Layout"))
		{
			ChangeLayoutRuntime("RANDOM");
		}

		// 房間狀態控制
		ImGui::Separator();
		ImGui::Text("Room State Control:");
		if (ImGui::Button("Force Combat State"))
		{
			SetState(RoomState::COMBAT);
		}
		ImGui::SameLine();
		if (ImGui::Button("Force Explored State"))
		{
			SetState(RoomState::EXPLORED);
		}
	}

	ImGui::End();
	*/
}

// === 公共 Debug 方法實現 ===
void MonsterRoom::DebugKillAllEnemies() { m_CombatManager.DebugKillAllEnemies(); }

void MonsterRoom::DebugKillCurrentWave() { m_CombatManager.DebugKillCurrentWaveEnemies(); }

// ===== CombatManager Debug UI 實現 =====
void MonsterRoom::CombatManager::DrawDebugUI()
{
	if (ImGui::CollapsingHeader("Combat Manager", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// === 戰鬥狀態信息 ===
		const char *stateNames[] = {"INACTIVE", "WAVE_ACTIVE", "WAVE_CLEARING", "COMPLETED"};
		ImGui::Text("Combat State: %s", stateNames[static_cast<int>(m_CombatState)]);
		ImGui::Text("Current Wave: %d / %d", m_CurrentWave + 1, GetTotalWaves());
		ImGui::Text("Alive Enemies: %d", GetAliveEnemiesInCurrentWave());

		// === 戰鬥控制按鈕 ===
		ImGui::Separator();
		if (ImGui::Button("Kill Current Wave Enemies"))
		{
			DebugKillCurrentWaveEnemies();
		}
		ImGui::SameLine();
		if (ImGui::Button("Kill All Enemies"))
		{
			DebugKillAllEnemies();
		}

		// === 門控制 ===
		ImGui::Separator();
		if (ImGui::Button("Open Doors"))
		{
			DoorOpenEvent openEvent;
			EventManager::GetInstance().Emit(openEvent);
		}
		ImGui::SameLine();
		if (ImGui::Button("Close Doors"))
		{
			DoorCloseEvent closeEvent;
			EventManager::GetInstance().Emit(closeEvent);
		}

		// === 詳細波次信息 ===
		if (ImGui::CollapsingHeader("Wave Details"))
		{
			for (int wave = 0; wave < GetTotalWaves(); ++wave)
			{
				bool isCurrentWave = (wave == m_CurrentWave);
				if (isCurrentWave)
					ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 0.0f, 1.0f)); // 黃色高亮

				int aliveCount = 0;
				int totalCount = 0;
				if (wave < static_cast<int>(m_AllWaveEnemies.size()))
				{
					const auto &waveEnemies = m_AllWaveEnemies[wave];
					totalCount = static_cast<int>(waveEnemies.size());
					for (const auto &weakEnemy : waveEnemies)
					{
						if (auto enemy = weakEnemy.lock())
						{
							if (enemy->IsActive())
								aliveCount++;
						}
					}
				}

				ImGui::Text("Wave %d: %d/%d alive", wave + 1, aliveCount, totalCount);

				if (isCurrentWave)
					ImGui::PopStyleColor();
			}
		}
	}
}

void MonsterRoom::CombatManager::DebugKillCurrentWaveEnemies()
{
	if (m_CurrentWave >= static_cast<int>(m_AllWaveEnemies.size()))
	{
		LOG_WARN("DebugKillCurrentWaveEnemies: Invalid current wave {}", m_CurrentWave);
		return;
	}

	const auto &currentWaveEnemies = m_AllWaveEnemies[m_CurrentWave];
	int killedCount = 0;

	for (const auto &weakEnemy : currentWaveEnemies)
	{
		if (auto enemy = weakEnemy.lock())
		{
			if (const auto &healthComp = enemy->GetComponent<HealthComponent>(ComponentType::HEALTH))
			{
				// 使用TakeDamage方法確保觸發死亡邏輯
				int currentHp = healthComp->GetCurrentHp();
				if (currentHp > 0)
				{
					healthComp->SetCurrentHp(0);
					// healthComp->TakeDamage(currentHp + 1000); // 造成足夠傷害確保死亡
					killedCount++;
				}
			}
		}
	}

	LOG_INFO("Debug: Killed {} enemies in current wave {}", killedCount, m_CurrentWave + 1);
}

void MonsterRoom::CombatManager::DebugKillAllEnemies()
{
	int totalKilled = 0;

	for (auto &waveEnemies : m_AllWaveEnemies)
	{
		for (const auto &weakEnemy : waveEnemies)
		{
			if (auto enemy = weakEnemy.lock())
			{
				if (const auto &healthComp = enemy->GetComponent<HealthComponent>(ComponentType::HEALTH))
				{
					// 使用TakeDamage方法確保觸發死亡邏輯
					int currentHp = healthComp->GetCurrentHp();
					healthComp->SetCurrentHp(0);
					if (currentHp > 0)
					{
						healthComp->SetCurrentHp(0);
						// healthComp->TakeDamage(currentHp + 1000); // 造成足夠傷害確保死亡
						totalKilled++;
					}
				}
			}
		}
	}

	// 直接結束戰鬥
	EndCombat();

	LOG_INFO("Debug: Killed all {} enemies and ended combat", totalKilled);
}
