//
// Created by tjx20 on 3/24/2025.
//

#ifndef STATECOMPONENT_HPP
#define STATECOMPONENT_HPP

#include "Components/Component.hpp"
#include "GameMechanism/StatusEffect.hpp"
#include "Components/AnimationComponent.hpp"

class StateComponent final: public Component {
public:
	explicit StateComponent();
	~StateComponent() override = default;

	void Init() override;
	void Update(){}

	/* ---Getter--- */
	State GetCurrentState()const { return m_currentState; }
	std::vector<StatusEffect> GetActiveEffects() { return m_StatusEffects; }

	/* ---Setter--- */
	void SetState(State newState);

	void applyStatusEffect(const StatusEffect& effect); // 添加狀態異常
	void updateStatusEffects(double deltaTime);			// 更新狀態異常

private:
	State m_currentState;
	std::vector<StatusEffect> m_StatusEffects;	// 狀態異常
};

#endif //STATECOMPONENT_HPP
