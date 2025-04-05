//
// Created by tjx20 on 3/24/2025.
//

#ifndef AICOMPONENT_HPP
#define AICOMPONENT_HPP

#include "Components/Component.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/MovementComponent.hpp"

class AIComponent : public Component {
public:
	explicit AIComponent(AIType type, int monsterPoint)
	: Component(ComponentType::AI), m_aiType(type), m_monsterPoint(monsterPoint) {}
	~AIComponent() override = default;

	void Init() override;
	void Update() override;

	// 行為模式接口
	virtual void OnDetectTarget() {} // 攻擊型專用
	virtual void OnSummonTimer() {}                  // 召喚型專用
	virtual void OnWanderUpdate() {}                 // 游蕩型專用

protected:
	AIType m_aiType;
	int m_monsterPoint;
	float m_behaviorTimer = 0;
};


#endif //AICOMPONENT_HPP