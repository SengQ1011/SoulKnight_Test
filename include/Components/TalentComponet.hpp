//
// Created by tjx20 on 3/29/2025.
//

#ifndef TALENTCOMPONET_HPP
#define TALENTCOMPONET_HPP

#include "Components/Component.hpp"
#include "GameMechanism/Talent.hpp" //因为GetTalents在HPP实作
// class Talent;


class TalentComponent : public Component
{
public:
	explicit TalentComponent();
	~TalentComponent() override = default;

	//----Getter----
	[[nodiscard]] std::vector<Talent> GetTalents() const;

	//----Setter----
	void AddTalent(const Talent &talent);
	void RemoveTalent(const Talent &talent);

private:
	std::vector<Talent> m_talents;
};


#endif //TALENTCOMPONET_HPP
