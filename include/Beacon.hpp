//
// Created by QuzzS on 2025/2/28.
//

#ifndef BEACON_HPP
#define BEACON_HPP

#include "pch.hpp"
#include "Util/Logger.hpp"

class Beacon {
public:
	Beacon() = default;
	~Beacon() = default;

	void Update(const glm::vec2& ReferenceOriginCoord = {}, const glm::vec2& ReferenceObjectCoord = {})
	{
		//等比分點
		m_WorldCoord = (ReferenceObjectCoord * 5.0f + ReferenceOriginCoord * 2.0f) / 7.0f; //靠近referenceObjectCoord，遠離referenceOriginCoord
	};

	[[nodiscard]] glm::vec2 GetBeaconWorldCoord() const
	{
		return m_WorldCoord;
	}

protected:
	glm::vec2 m_WorldCoord;
};

#endif //BEACON_HPP
