//
// Created by QuzzS on 2025/3/4.
//

#ifndef SKILL_HPP
#define SKILL_HPP

#include <string>
#include "Creature/Character.hpp"

class Skill {
public:
	Skill(const std::string &name, const float cooldownTime): m_skillName(name), m_CDTime(cooldownTime) {};
	virtual ~Skill() = default;

	virtual void SkillUpdate() = 0;

	//----Getter----
	[[nodiscard]] std::string GetSkillName() const { return m_skillName; }
	[[nodiscard]] float GetCoolDownTime() const { return m_CDTime; }
	[[nodiscard]] float GetremainingCooldown() const { return m_remainingCD; }
	[[nodiscard]] bool IsReady() const { return m_remainingCD <= 0; }
	[[nodiscard]] bool GetSkillActive() const { return isActive; }

	//----Setter----
	void SetCoolDownTime(float cooldownTime) { m_CDTime = cooldownTime; }
	void SetSkillActive(bool active) { isActive = active; }

	virtual void Execute() = 0;

protected:
	std::string m_skillName;
	float m_CDTime;
	float m_remainingCD;
	bool isActive = false;
};

#endif