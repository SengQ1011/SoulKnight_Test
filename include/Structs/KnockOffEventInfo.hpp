//
// Created by tjx20 on 5/15/2025.
//

#ifndef KNOCKOFFEVENTINFO_HPP
#define KNOCKOFFEVENTINFO_HPP
#include <glm/vec2.hpp>
#include "EventInfo.hpp"

struct KnockOffEventInfo final : TypedEventInfo<KnockOffEventInfo>
{
	glm::vec2 impulseVelocity = glm::vec2(0.0f, 0.0f);

	explicit KnockOffEventInfo(const glm::vec2 velocity) :
		TypedEventInfo(EventType::KnockOff), impulseVelocity(velocity) {}
};

#endif //KNOCKOFFEVENTINFO_HPP
