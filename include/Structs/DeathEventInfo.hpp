//
// Created by QuzzS on 2025/4/29.
//

#ifndef DEATHEVENTINFO_HPP
#define DEATHEVENTINFO_HPP

#include <memory>
#include "EventInfo.hpp"


class Character;
struct DeathEventInfo : TypedEventInfo<DeathEventInfo> {
	std::weak_ptr<Character> m_Corpse; //尸體

	explicit DeathEventInfo(const std::shared_ptr<Character>& who)
		: TypedEventInfo(EventType::Death), m_Corpse(who) {}
};

#endif //DEATHEVENTINFO_HPP
