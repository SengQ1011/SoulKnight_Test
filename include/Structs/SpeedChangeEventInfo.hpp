//
// Created by tjx20 on 6/12/2025.
//

#ifndef SPEEDCHANGEEVENTINFO_HPP
#define SPEEDCHANGEEVENTINFO_HPP

#include "EventInfo.hpp"

struct SpeedChangeEventInfo final : TypedEventInfo<SpeedChangeEventInfo>
{
	float speedRatio;
	float durationTime;

	explicit SpeedChangeEventInfo(const float ratio, const float time) :
		TypedEventInfo(EventType::SpeedChanged),
		speedRatio(ratio), durationTime(time) {}
};

#endif //SPEEDCHANGEEVENTINFO_HPP
