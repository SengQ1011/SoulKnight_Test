//
// Created by tjx20 on 3/29/2025.
//

#include "Skill/FullFirepower.hpp"
#include "Util/Time.hpp"
#include "Components/AttackComponent.hpp"

FullFirepower::FullFirepower(std::string name, float cooldownTime, float skillTime) : Skill(name, cooldownTime) {}

void FullFirepower::SkillUpdate(){
	float deltaTime = Util::Time::GetDeltaTimeMs() /1000.0f;
	// 技能持续时间倒计时
	if (isActive) {
		m_remainingCD -= deltaTime;
		if (m_remainingCD <= 0.0f) {
			// 结束双持状态
			//auto attackComp = m_owner.GetComponent<AttackComponent>(ComponentType::ATTACK);
			//attackComp.SetDualWield(false);
			isActive = false;
		}
	}

}

void FullFirepower::Execute()
{

}