//
// Created by tjx20 on 3/29/2025.
//

#ifndef TALENTCOMPONET_HPP
#define TALENTCOMPONET_HPP

#include "Components/Component.hpp"
#include "GameMechanism/Talent.hpp"
#include "Util/GameObject.hpp"

class TalentComponent : public Component {
public:
	explicit TalentComponent();
	~TalentComponent() override = default;

	//----Getter----
	[[nodiscard]]std::vector<Talent> GetTalents() const { return m_talents; }

	//----Setter----
	void AddTalent(const Talent& talent);
	void RemoveTalent(const Talent& talent);

private:
	std::vector<Talent> m_talents;
};

#endif //TALENTCOMPONET_HPP
