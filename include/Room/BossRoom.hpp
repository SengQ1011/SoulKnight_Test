//
// Created by QuzzS on 2025/4/25.
//

#ifndef BOSSROOM_HPP
#define BOSSROOM_HPP

#include "DungeonRoom.hpp"
#include "EnumTypes.hpp"
#include "ObserveManager/EventManager.hpp"
#include "Structs/DeathEventInfo.hpp"


// 前向聲明
class UISlider;

class BossRoom final : public DungeonRoom
{
public:
	explicit BossRoom(const glm::vec2 worldCoord, const std::shared_ptr<Loader> &loader,
					  const std::shared_ptr<RoomObjectFactory> &room_object_factory,
					  const glm::vec2 &mapGridPos = glm::vec2(0), const RoomType roomType = RoomType::BOSS) :
		DungeonRoom(worldCoord, loader, room_object_factory, mapGridPos, roomType), m_CombatManager(this),
		m_BossHealthUI(this)
	{
		// 訂閱敵人死亡事件
		m_EnemyDeathListenerID = EventManager::GetInstance().Subscribe<EnemyDeathEvent>(
			[this](const EnemyDeathEvent &event) { this->OnEnemyDeathEvent(event); });
	}

	~BossRoom() override
	{
		// 取消訂閱敵人死亡事件
		try
		{
			if (m_EnemyDeathListenerID != 0)
			{
				EventManager::GetInstance().Unsubscribe<EnemyDeathEvent>(m_EnemyDeathListenerID);
			}
		}
		catch (...)
		{
			LOG_WARN("BossRoom::~BossRoom: Exception occurred during destruction");
		}
	}

	void Start(const std::shared_ptr<Character> &player) override;
	void Update() override;
	void LoadFromJSON() override;
	void TryActivateByPlayer() override;
	void OnStateChanged() override;

	// === 事件處理方法 ===
	void OnEnemyDeathEvent(const struct EnemyDeathEvent &event);
	void AddEnemy(const std::shared_ptr<Character> &enemy);

private:
	// === Boss血量UI管理器 ===
	class BossHealthUIManager
	{
	public:
		explicit BossHealthUIManager(BossRoom *room);
		~BossHealthUIManager();

		void Initialize();
		void Update();
		void Show();
		void Hide();
		void SetBoss(const std::shared_ptr<Character> &boss);

	private:
		BossRoom *m_Room;
		std::shared_ptr<Character> m_Boss;

		// UI元件
		std::shared_ptr<nGameObject> m_BossNameText;
		std::shared_ptr<UISlider> m_BossHealthSlider;
		std::shared_ptr<nGameObject> m_HealthTrack;
		std::vector<std::shared_ptr<nGameObject>> m_UIObjects;

		// 監聽Boss血量的函數
		std::function<float()> m_ListenBossHealth;

		bool m_IsVisible = false;
		const float UI_SCALE = 1.0f; // UI放大倍數
	};

	// === 戰鬥管理器 ===
	class CombatManager
	{
	public:
		explicit CombatManager(BossRoom *room);

		void Initialize();
		void Update();

		// 戰鬥控制
		void StartCombat();
		void EndCombat();
		bool IsInCombat() const { return m_CombatState == CombatState::WAVE_ACTIVE; }
		bool IsCompleted() const { return m_CombatState == CombatState::COMPLETED; }

		// Boss管理
		void SpawnBoss();
		void OnBossDied();
		std::shared_ptr<Character> GetBoss() const { return m_Boss.lock(); }

	private:
		BossRoom *m_Room;
		CombatState m_CombatState = CombatState::INACTIVE;
		std::weak_ptr<Character> m_Boss;

		void CheckBossCompletion();
	};

	// === 私有方法 ===
	void CreatePortal();
	void CreateTreasureChests();
	void CloseDoors();
	void OpenDoors();
	void ShowPortalAndChests();

	// === 成員變量 ===
	CombatManager m_CombatManager;
	BossHealthUIManager m_BossHealthUI;

	// 房間物件
	std::shared_ptr<nGameObject> m_Portal;
	std::shared_ptr<nGameObject> m_WeaponChest;
	std::shared_ptr<nGameObject> m_RewardChest;

	// 事件監聽器ID
	EventManager::ListenerID m_EnemyDeathListenerID;
};

#endif // BOSSROOM_HPP
