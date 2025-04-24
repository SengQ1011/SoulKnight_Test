//
// Created by tjx20 on 4/2/2025.
//

#include "Components/SkillComponent.hpp"
#include "Skill/Skill.hpp"


void SkillComponent::Update()
{
	{ m_skill->SkillUpdate(); }
}

bool SkillComponent::ExecuteSkill()
{
	if (m_skill->skillIsReady())
	{
		m_skill->SetSkillActive(true);
		m_skill->Execute();
		return true;
	}
	return false;
}