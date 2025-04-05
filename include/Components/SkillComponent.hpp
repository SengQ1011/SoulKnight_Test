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
		m_skill->SkillUpdate();
	}

	bool ExecuteSkill() {
		if (m_skill->skillIsReady()) {
			m_skill->SetSkillActive(true);
			m_skill->Execute();
			return true;
		}
		return false;
	}

private:
	std::shared_ptr<Skill> m_skill;
};

#endif //SKILLCOMPONENT_HPP
