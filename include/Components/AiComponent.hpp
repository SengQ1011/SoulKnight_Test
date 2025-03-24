//
// Created by tjx20 on 3/24/2025.
//

#ifndef AICOMPONENT_HPP
#define AICOMPONENT_HPP

enum class AIType {
	CHASER,  // 追蹤玩家
	SHOOTER, // 遠程射擊
	PATROLLER // 巡邏
};

class AIComponent : public Component {
public:
	explicit AIComponent(AIType type) : aiType(type) {}

	void UpdateAI() {
		switch (aiType) {
		case AIType::CHASER:
			break;
		case AIType::SHOOTER:
			break;
		case AIType::PATROLLER:
			break;
		}
	}

private:
	AIType aiType;
};


#endif //AICOMPONENT_HPP