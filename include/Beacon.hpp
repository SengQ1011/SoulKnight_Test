//
// Created by QuzzS on 2025/2/28.
//

#ifndef BEACON_HPP
#define BEACON_HPP

#include "pch.hpp"
#include "Util/Logger.hpp"
#include "Util/Input.hpp"

class Beacon {
public:
	Beacon() = default;
	~Beacon() = default;

	void Update(const glm::vec2& ReferenceOriginCoord = {}, const glm::vec2& ReferenceObjectCoord = {})
	{
		//等比分點
		m_WorldCoord = (ReferenceObjectCoord * 4.0f + ReferenceOriginCoord * 3.0f) / 7.0f; //靠近referenceObjectCoord，遠離referenceOriginCoord

		//TODO: 限制範圍 可能需要變數 範圍還沒限制到
		// const glm::vec2 offSet = ReferenceObjectCoord - ReferenceOriginCoord;
		// if (m_WorldCoord.x > ReferenceOriginCoord.x + u) {m_WorldCoord.x = ReferenceOriginCoord.x + u;}
		// if (m_WorldCoord.x < ReferenceOriginCoord.x - u) {m_WorldCoord.x = ReferenceOriginCoord.x - u;}
		// if (m_WorldCoord.y > ReferenceOriginCoord.y + v) {m_WorldCoord.y = ReferenceOriginCoord.y + v;}
		// if (m_WorldCoord.y < ReferenceOriginCoord.y - v) {m_WorldCoord.y = ReferenceOriginCoord.y - v;}
		// if (Util::Input::IsKeyUp(Util::Keycode::MOUSE_RB))
		// {
		// 	LOG_DEBUG("cursor {} {} ", m_WorldCoord, glm::vec2(1 - offSet.x * offSet.x / 2,1 - offSet.y * offSet.y / 2));
		// }
	};

	[[nodiscard]] glm::vec2 GetBeaconWorldCoord() const
	{
		return m_WorldCoord;
	}

protected:
	glm::vec2 m_WorldCoord;
};

#endif //BEACON_HPP
