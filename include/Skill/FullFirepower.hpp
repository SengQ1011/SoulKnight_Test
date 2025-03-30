//
// Created by tjx20 on 3/29/2025.
//

#ifndef FULLFIREPOWER_HPP
#define FULLFIREPOWER_HPP

#include "Skill/Skill.hpp"

class FullFirepower : public Skill {
public:
	explicit FullFirepower(Character& owner, std::string name, float cooldownTime, float skillTime);
	~FullFirepower() override = default;

	void SkillUpdate() override;
	void Execute() override;

private:
	float m_skillTime = 5.0f;
	float m_Timer;
};

#endif //FULLFIREPOWER_HPP
