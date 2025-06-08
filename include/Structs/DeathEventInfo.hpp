//
// Created by QuzzS on 2025/4/29.
//

#ifndef DEATHEVENTINFO_HPP
#define DEATHEVENTINFO_HPP

#include <memory>
#include "EventInfo.hpp"
#include "glm/glm.hpp"

class Character;

// 角色死亡事件（內部Component間通信）
struct DeathEventInfo : TypedEventInfo<DeathEventInfo>
{
	std::weak_ptr<Character> m_Corpse; // 尸體

	explicit DeathEventInfo(const std::shared_ptr<Character> &who) : TypedEventInfo(EventType::Death), m_Corpse(who) {}
};

// 敵人死亡事件（全局事件，用於通知MonsterRoom等）
struct EnemyDeathEvent : TypedEventInfo<EnemyDeathEvent>
{
	std::weak_ptr<Character> m_DeadEnemy;
	glm::vec2 m_DeathPosition;

	explicit EnemyDeathEvent(const std::shared_ptr<Character> &enemy);
};

#endif // DEATHEVENTINFO_HPP
