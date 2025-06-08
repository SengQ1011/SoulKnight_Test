//
// Created by QuzzS on 2025/4/27.
//

#ifndef EVENTINFO_HPP
#define EVENTINFO_HPP

#include <typeindex>

// EventManager 和 Component分兩類？
enum class EventType
{
	None,

	// 大概
	Collision,
	TakeDamage,

	// 板機廣播用
	TriggerEnter,
	TriggerStay,
	TriggerExit,

	Click,

	// AttackTrigger
	BlockedProjectileBySword,
	ReflectProjectile,
	KnockOff,

	// 通用
	Death,
	PositionChanged,
	ValueChanged,

	// Camera相關
	CameraShake,

	// 視覺特效
	StartFlicker,

	// 角色動作
	EnemyDeath,

	// 門相關
	DoorOpen,
	DoorClose,

	// 破壞物件相關
	BoxBreak
	// HealthChanged,
	// EnergyChanged,
	// ArmorBroken,

	// PlayerDeath,
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

struct EventInfo
{
	explicit EventInfo(const EventType type) : m_EventType(type) {}
	virtual ~EventInfo() = default;

	virtual std::type_index GetType() const { return typeid(void); }; // 信息類型
	EventType GetEventType() const { return m_EventType; } // 事件名稱
	void SetEventType(const EventType type) { m_EventType = type; } // 手動設置特定事件

private:
	EventType m_EventType = EventType::None;
};

template <typename Derived>
struct TypedEventInfo : EventInfo
{
	explicit TypedEventInfo(const EventType type) : EventInfo(type) {}
	~TypedEventInfo() override = default;
	std::type_index GetType() const override { return typeid(Derived); }
};

// ===== 具體事件結構定義 =====

// Camera抖動事件
struct CameraShakeEvent : TypedEventInfo<CameraShakeEvent>
{
	float duration;
	float intensity;

	CameraShakeEvent(float dur = 0.3f, float intens = 10.0f) :
		TypedEventInfo(EventType::CameraShake), duration(dur), intensity(intens)
	{
	}
};

// 閃爍特效事件
struct StartFlickerEvent : TypedEventInfo<StartFlickerEvent>
{
	float duration;
	float interval;

	StartFlickerEvent(float dur = 0.5f, float inter = 0.1f) :
		TypedEventInfo(EventType::StartFlicker), duration(dur), interval(inter)
	{
	}
};

// 門開啟事件
struct DoorOpenEvent : TypedEventInfo<DoorOpenEvent>
{
	DoorOpenEvent() : TypedEventInfo(EventType::DoorOpen) {}
};

// 門關閉事件
struct DoorCloseEvent : TypedEventInfo<DoorCloseEvent>
{
	DoorCloseEvent() : TypedEventInfo(EventType::DoorClose) {}
};

// 箱子破壞事件
struct BoxBreakEvent : TypedEventInfo<BoxBreakEvent>
{
	BoxBreakEvent() : TypedEventInfo(EventType::BoxBreak) {}
};

#endif // EVENTINFO_HPP
