//
// Created by tjx20 on 3/29/2025.
//

#ifndef SKILLCOMPONENT_HPP
#define SKILLCOMPONENT_HPP

#include <utility>

#include "Components/Component.hpp"
class Skill;


class SkillComponent : public Component
{
public:
	explicit SkillComponent(std::shared_ptr<Skill> skill) : m_skill(std::move(skill)) {};
	~SkillComponent() override = default;

	void Update() override;

	bool ExecuteSkill();

private:
	std::shared_ptr<Skill> m_skill;
};


#endif //SKILLCOMPONENT_HPP
