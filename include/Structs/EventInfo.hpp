//
// Created by QuzzS on 2025/4/27.
//

#ifndef EVENTINFO_HPP
#define EVENTINFO_HPP

#include <typeindex>

// EventManager 和 Component分兩類？
enum class EventType {
	None,

	// 大概
	Collision,
	TakeDamage,

	// 板機廣播用
	TriggerEnter,
	TriggerStay,
	TriggerExit,

	// AttackTrigger
	BlockedProjectileBySword,
	ReflectProjectile,

	// 通用
	Death,
	// HealthChanged,
	// EnergyChanged,
	// ArmorBroken,

	// 角色動作
	// PlayerDeath,
	// EnemyDeath,
	// PlayerSpawned,
	// EnemySpawned,
	// UseSkill,

	// 房間
	// RoomCleared,
	// BossDefeated,
	// RoomEntered,

	// UI
	// UpdateHealthUI,
	// UpdateEnergyUI,
	// ShowGameOverScreen,

	// 自由擴充...
};

struct EventInfo {
	explicit EventInfo(const EventType type): m_EventType(type) {}
	virtual ~EventInfo() = default;

	virtual std::type_index GetType() const = 0;			// 信息類型
	EventType GetEventType() const { return m_EventType;}	// 事件名稱
	void SetEventType(const EventType type) { m_EventType = type; } // 手動設置特定事件

private:
	EventType m_EventType = EventType::None;
};

template<typename Derived>
struct TypedEventInfo : EventInfo {
	explicit TypedEventInfo(const EventType type): EventInfo(type) {}
	~TypedEventInfo() override = default;
	std::type_index GetType() const override { return typeid(Derived); }
};

#endif //EVENTINFO_HPP
