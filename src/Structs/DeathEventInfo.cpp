//
// Created by QuzzS on 2025/4/29.
//

#include "Structs/DeathEventInfo.hpp"
#include "Creature/Character.hpp"

EnemyDeathEvent::EnemyDeathEvent(const std::shared_ptr<Character> &enemy) :
	TypedEventInfo(EventType::EnemyDeath), m_DeadEnemy(enemy)
{
	if (enemy)
	{
		m_DeathPosition = enemy->m_WorldCoord;
	}
}
