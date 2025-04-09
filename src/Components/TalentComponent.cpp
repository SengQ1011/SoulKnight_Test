//
// Created by tjx20 on 4/5/2025.
//

#include "Components/TalentComponet.hpp"
#include "Creature/Character.hpp"

TalentComponent::TalentComponent(){}

void TalentComponent::AddTalent(const Talent& talent) {
 	auto character = GetOwner<Character>();
	m_talents.push_back(talent);
	talent.Apply(*character); // 立刻對物件施加天賦效果
 }

void TalentComponent::RemoveTalent(const Talent& talent) {
	auto character = GetOwner<Character>();
	auto it = std::remove_if(m_talents.begin(), m_talents.end(),
							 [&talent](const Talent& t) { return t.GetName() == talent.GetName(); });

	if (it != m_talents.end()) {
		it->Undo(*character);  // 撤銷天賦效果
		m_talents.erase(it, m_talents.end());
	}
}