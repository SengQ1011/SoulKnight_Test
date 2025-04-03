//
// Created by tjx20 on 3/29/2025.
//

#ifndef SKILLCOMPONENT_HPP
#define SKILLCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Skill/Skill.hpp"

class SkillComponent : public Component {
public:
	explicit SkillComponent(std::shared_ptr<Skill> skill);
	~SkillComponent() override = default;

	void Update() override {
		if (m_skill->GetSkillActive()) {
			m_skill->SkillUpdate();
		}
	}

	void ExecuteSkill() {
		if (m_skill->GetremainingCooldown() <= 0) {
			m_skill->Execute();
		}
	}

private:
	std::shared_ptr<Skill> m_skill;
};

#endif //SKILLCOMPONENT_HPP
