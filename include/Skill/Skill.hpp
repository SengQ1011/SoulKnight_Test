//
// Created by QuzzS on 2025/3/4.
//

#ifndef SKILL_HPP
#define SKILL_HPP

#include <string>

#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "Util/Time.hpp"

class Skill {
public:
	Skill(std::weak_ptr<Character> owner, const std::string &name, const std::string &iconPath, const float skillDuration, const float cooldownTime)
	: m_owner(owner), m_skillName(name), m_iconPath(iconPath),
	m_skillDuration(skillDuration), m_remainingDuration(skillDuration),
	m_CDTime(cooldownTime), m_remainingCD(0) {};

	virtual ~Skill() = default;

	void SkillUpdate(){
		float deltaTime = Util::Time::GetDeltaTimeMs() /1000.0f;
		// 技能持续时间倒计时
		if (isActive) {
			m_remainingDuration -= deltaTime;
			if (m_remainingDuration <= 0.0f) {
				this->EndSkill();
				if (m_owner.lock()->GetCharacterName() == "Knight") {
					m_owner.lock()->GetComponent<AnimationComponent>(ComponentType::ANIMATION)->SetSkillEffect(false);
				}
				isActive = false;
				m_remainingCD = m_CDTime;
			}
		} else {
			if (m_remainingCD > 0.0f) {
				m_remainingCD -= deltaTime;
			}
		}
	}
	virtual void EndSkill() = 0;

	//----Getter----
	[[nodiscard]] std::string GetSkillName() const { return m_skillName; }
	[[nodiscard]] std::string GetIconPath() const { return m_iconPath; }
	[[nodiscard]] float GetCDTime() const { return m_CDTime; }
	[[nodiscard]] float GetremainingCD() const { return m_remainingCD; }
	[[nodiscard]] bool skillIsReady() const { return m_remainingCD <= 0; }
	[[nodiscard]] bool GetSkillActive() const { return isActive; }

	//----Setter----
	void SetCoolDownTime(float cooldownTime) { m_CDTime = cooldownTime; }
	void SetSkillActive(bool active) { isActive = active; }

	virtual void Execute() = 0;

protected:
	std::weak_ptr<Character> m_owner;
	std::string m_skillName;
	std::string m_iconPath;
	float m_skillDuration;		// 技能持續時間
	float m_remainingDuration;  // 剩餘持續時間
	float m_CDTime;				// 固定技能CD時間
	float m_remainingCD;		// 剩餘CD時間
	bool isActive = false;
};

#endif