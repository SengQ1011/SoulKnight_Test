//
// Created by tjx20 on 3/24/2025.
//

#ifndef STATECOMPONENT_HPP
#define STATECOMPONENT_HPP

#include "Components/Component.hpp"
#include "GameMechanism/StatusEffect.hpp"

enum class State
{
	STANDING,
	MOVING,
	ATTACK,
	DEAD
};

class StateComponent final: public Component {
public:
	explicit StateComponent();
	~StateComponent() override = default;

	void Update(){}

	/* ---Getter--- */
	State GetState()const { return m_currentState; }
	State GetPrevState() { return m_prevState; }       // 上一个状态
	std::vector<StatusEffect> GetActiveEffects() { return m_StatusEffects; }

	/* ---Setter--- */
	void SetState(State newState);
	void SetPrevState(State State);
	void applyStatusEffect(const StatusEffect& effect); // 添加狀態異常
	void updateStatusEffects(double deltaTime);			// 更新狀態異常

private:
	State m_currentState;
	State m_prevState;
	std::vector<StatusEffect> m_StatusEffects;	// 狀態異常
};

#endif //STATECOMPONENT_HPP
