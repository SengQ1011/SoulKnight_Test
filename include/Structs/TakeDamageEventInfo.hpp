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

	explicit TakeDamageEventInfo(const int id, const int dmg) :
		TypedEventInfo(EventType::TakeDamage),m_Id(id), damage(dmg) {}
};

#endif //TAKEDAMAGEEVENTINFO_HPP
