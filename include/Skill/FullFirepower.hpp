//
// Created by tjx20 on 3/29/2025.
//

#ifndef FULLFIREPOWER_HPP
#define FULLFIREPOWER_HPP

#include "Skill/Skill.hpp"

class FullFirepower : public Skill {
public:
	explicit FullFirepower(const std::weak_ptr<Character> &m_owner, const std::string& name,
						   const std::string &iconPath, float skillDuration, float cooldownTime);
	~FullFirepower() override = default;

	void EndSkill() override;
	void Execute() override;

private:

};

#endif //FULLFIREPOWER_HPP
