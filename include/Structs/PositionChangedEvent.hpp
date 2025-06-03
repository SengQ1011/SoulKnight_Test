//
// Created by QuzzS on 2025/5/22.
//

#ifndef POSITIONCHANGEDEVENT_HPP
#define POSITIONCHANGEDEVENT_HPP

#include "EventInfo.hpp"
#include "glm/vec2.hpp"

struct PositionChangedEvent : TypedEventInfo<PositionChangedEvent> {
	glm::vec2 m_NewPosition;

	explicit PositionChangedEvent(const glm::vec2& newPosition)
		: TypedEventInfo(EventType::PositionChanged), m_NewPosition(newPosition) {}
};

#endif //POSITIONCHANGEDEVENT_HPP
