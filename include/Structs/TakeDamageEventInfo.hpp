//
// Created by QuzzS on 2025/4/29.
//

#ifndef TAKEDAMAGEEVENTINFO_HPP
#define TAKEDAMAGEEVENTINFO_HPP
#include "EventInfo.hpp"

struct TakeDamageEventInfo final : TypedEventInfo<TakeDamageEventInfo>
{
	int m_Id = 0;
	int damage = 0;
	StatusEffect elementalDamage;
	bool isCriticalHit = false;

	explicit TakeDamageEventInfo(const int id, const int dmg, const StatusEffect &elementalDamage,
								 const bool isCrit = false) :
		TypedEventInfo(EventType::TakeDamage), m_Id(id), damage(dmg), elementalDamage(elementalDamage),
		isCriticalHit(isCrit)
	{
	}
};

#endif // TAKEDAMAGEEVENTINFO_HPP
