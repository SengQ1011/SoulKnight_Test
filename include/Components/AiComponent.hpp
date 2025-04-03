//
// Created by tjx20 on 3/24/2025.
//

#ifndef AICOMPONENT_HPP
#define AICOMPONENT_HPP

#include "Components/Component.hpp"

class AIComponent : public Component {
public:
	explicit AIComponent(AIType type, int monsterPoint) : aiType(type) {}

	void UpdateAI() {
		switch (aiType) {
		case AIType::ATTACK:
			break;
		case AIType::SUMMON:
			break;
		case AIType::WANDER:
			break;
		}
	}

private:
	AIType aiType;
	int m_monsterPoint;
};


#endif //AICOMPONENT_HPP