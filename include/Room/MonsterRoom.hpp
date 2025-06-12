//
// Created by QuzzS on 2025/4/23.
//

#ifndef MONSTERROOM_HPP
#define MONSTERROOM_HPP

#include "DungeonRoom.hpp"
#include "ObserveManager/EventManager.hpp"
#include "Structs/DeathEventInfo.hpp"

// 前向聲明
struct WaveConfig;

class MonsterRoom final : public DungeonRoom
{
public:
	explicit MonsterRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
						 const std::shared_ptr<RoomObjectFactory> &room_object_factory,
						 const glm::vec2 &mapGridPos = glm::vec2(0), const RoomType roomType = RoomType::MONSTER) :
		DungeonRoom(worldCoord, loader, room_object_factory, mapGridPos, roomType), m_CombatManager(this)
	{
		// 訂閱敵人死亡事件
		m_EnemyDeathListenerID = EventManager::GetInstance().Subscribe<EnemyDeathEvent>(
			[this](const EnemyDeathEvent &event) { this->OnEnemyDeathEvent(event); });
	}

	~MonsterRoom() override
	{
		// 取消訂閱敵人死亡事件
		// 添加安全檢查，避免在程序結束時存取已銷毀的EventManager
		try
		{
			if (m_EnemyDeathListenerID != 0)
			{
				EventManager::GetInstance().Unsubscribe<EnemyDeathEvent>(m_EnemyDeathListenerID);
			}
		}
		catch (...)
		{
			LOG_WARN("MonsterRoom::~MonsterRoom: Exception occurred during destruction");
		}
	}

	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;

	void LoadFromJSON() override;

	// 測試用：載入指定佈局
	void LoadFromJSON_Specific(const std::string &layoutName);

	// 運行時佈局更換功能
	void ChangeLayoutRuntime(const std::string &layoutName = "");
	bool CanChangeLayout() const;

	void TryActivateByPlayer() override;
	void OnStateChanged() override;

	// === 事件處理方法 ===
	void OnEnemyDeathEvent(const struct EnemyDeathEvent &event);
	void AddEnemy(const std::shared_ptr<Character> &enemy);

	// === Debug UI 功能 ===
	void DrawDebugUI(); // 繪製房間Debug UI（包含CombatManager的Debug UI）

private:
	// === 戰鬥配置結構 ===
	struct WaveConfig
	{
		int enemyCount;
		std::vector<int> enemyTypes; // 敵人ID列表
		float spawnDelay = 0.5f; // 生成間隔
	};

	// === 戰鬥管理器 ===
	class CombatManager
	{
	public:
		explicit CombatManager(MonsterRoom *room);

		void Initialize(const std::vector<WaveConfig> &waveConfigs);
		void Update();

		// 戰鬥控制
		void StartCombat();
		void EndCombat();
		bool IsInCombat() const
		{
			return m_CombatState != CombatState::INACTIVE && m_CombatState != CombatState::COMPLETED;
		}
		bool IsCompleted() const { return m_CombatState == CombatState::COMPLETED; }

		// 波次管理
		void StartCurrentWave();
		void OnEnemyDied();

		// 新增方法
		void AddWaveEnemies(int waveIndex, const std::vector<std::shared_ptr<Character>> &enemies);
		void ActivateCurrentWaveEnemies();
		void AddEnemyToCurrentWave(const std::shared_ptr<Character> &enemy);

		// 狀態查詢
		CombatState GetState() const { return m_CombatState; }
		int GetCurrentWave() const { return m_CurrentWave; }
		int GetTotalWaves() const { return static_cast<int>(m_WaveConfigs.size()); }
		int GetAliveEnemiesInCurrentWave() const;

		// === Debug UI 功能 ===
		void DrawDebugUI(); // 繪製Debug UI介面
		void DebugKillCurrentWaveEnemies(); // 一鍵殺光當前波次敵人
		void DebugKillAllEnemies(); // 一鍵殺光全部敵人結束戰鬥

	private:
		MonsterRoom *m_Room;
		CombatState m_CombatState = CombatState::INACTIVE;

		// 波次配置
		std::vector<WaveConfig> m_WaveConfigs;
		int m_CurrentWave = 0;

		// 所有波次的敵人（預先生成）
		std::vector<std::vector<std::weak_ptr<Character>>> m_AllWaveEnemies;

		// 輔助方法
		void CheckWaveCompletion();
		void StartNextWave();
		void CleanupDeadEnemies();
		void SetEnemyVisible(const std::shared_ptr<Character> &enemy, bool visible); // 同時控制敵人和武器的可見性
		void HideAllEnemies(); // 隱藏所有波次的敵人
	};

	// === 私有方法 ===
	std::shared_ptr<Character> SpawnEnemy(int enemyType, glm::vec2 position);
	void SetEnemyVisible(const std::shared_ptr<Character> &enemy, bool visible); // 同時控制敵人和武器的可見性
	void LoadCombatConfiguration();
	void PreSpawnAllWaveEnemies(); // 預先生成所有波次的怪物
	std::vector<glm::vec2> GenerateSpawnPositions(int count); // 生成隨機位置（向後兼容）
	std::vector<glm::vec2> GenerateSpawnPositionsWithSize(int count,
														  const glm::vec2 &entitySize); // 根據實體大小生成位置
	std::vector<glm::vec2> GenerateSpawnPositionsWithSizeAndExclusions(
		int count, const glm::vec2 &entitySize,
		const std::vector<glm::ivec2> &occupiedPositions); // 根據實體大小生成位置並避開已佔用區域
	bool IsAreaClearForEntity(const glm::ivec2 &gridPos, int entityGridWidth,
							  int entityGridHeight) const; // 檢查區域是否足夠容納實體
	bool IsAreaOverlapWithOccupied(const glm::ivec2 &gridPos, int entityGridWidth, int entityGridHeight,
								   const std::vector<glm::ivec2> &occupiedPositions) const; // 檢查是否與已佔用位置重疊
	void RecordOccupiedPositions(const glm::vec2 &worldPos, const glm::vec2 &entitySize,
								 std::vector<glm::ivec2> &occupiedPositions); // 記錄實體佔用的網格位置
	void CloseDoors();
	void OpenDoors();
	void SpawnRewardChest();

	// 佈局更換相關輔助方法
	void ClearNonEssentialObjects(); // 清理非關鍵物件
	void RebuildConnectionsAndWalls(); // 重建通道和牆壁
	void RepositionEntitiesAfterLayoutChange(); // 重新定位實體位置
	void RepositionEnemies(); // 重新定位怪物
	void RepositionPlayerIfNeeded(); // 重新定位玩家（如果需要）

	// === 成員變量 ===
	CombatManager m_CombatManager;
	std::vector<WaveConfig> m_WaveConfigs; // 從JSON加載的波次配置

	// 事件監聽器ID
	EventManager::ListenerID m_EnemyDeathListenerID;
};

#endif // MONSTERROOM_HPP
