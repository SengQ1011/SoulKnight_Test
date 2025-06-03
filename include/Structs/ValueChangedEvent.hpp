//
// Created by QuzzS on 2025/5/23.
//

#ifndef VALUECHANGEDEVENT_HPP
#define VALUECHANGEDEVENT_HPP

#include "EventInfo.hpp"

struct ValueChangedEvent : TypedEventInfo<ValueChangedEvent> {
	float m_CurrentValue, m_MaxValue;

	explicit ValueChangedEvent(const float newPosition, const float maxValue)
		: TypedEventInfo(EventType::ValueChanged), m_CurrentValue(newPosition), m_MaxValue(maxValue) {}
};

#endif //VALUECHANGEDEVENT_HPP
