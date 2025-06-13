//
// Created by tjx20 on 4/2/2025.
//

#include "Components/SkillComponent.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Skill/Skill.hpp"


void SkillComponent::Update()
{
	{
		m_skill->SkillUpdate();
	}
}

bool SkillComponent::ExecuteSkill()
{
	if (m_skill->skillIsReady())
	{
		// 根據技能名稱播放對應的音效
		const std::string &skillName = m_skill->GetSkillName();
		if (skillName == "Full Firepower")
		{
			AudioManager::GetInstance().PlaySFX("player_skill");
		}
		// TODO:未來可以在這裡加入其他技能的音效
		// else if (skillName == "other skill")
		// {
		//     AudioManager::GetInstance().PlaySFX("other skill sound");
		// }

		m_skill->SetSkillActive(true);
		m_skill->Execute();
		return true;
	}
	return false;
}
