//
// Created by tjx20 on 4/18/2025.
//

#include "GameMechanism/TalentDatabase.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"

std::vector<Talent> CreateTalentList() {
	std::vector<Talent> list;

	list.emplace_back(1,"破甲保護", "/UI/ui_talentIcon/ui_buff_18.png", [](Character& obj) {
			if (auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetBreakProtection(true);
			}
		},
		[](Character& obj) {
			if (auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetBreakProtection(false);
			}
		});

	list.emplace_back(2, "護甲+1", "/UI/ui_talentIcon/ui_buff_28.png", [](Character& obj){
			if (const auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetMaxArmor(healthComp->GetMaxArmor() + 1);
			}
		},
		[](Character& obj) {
			if (const auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetMaxArmor(healthComp->GetMaxArmor() - 1);
			}
		});

	list.emplace_back(3, "子彈反彈", "/UI/ui_talentIcon/ui_buff_24.png", [](Character& obj){
			if (const auto attackComp = obj.GetComponent<AttackComponent>(ComponentType::ATTACK)) {
				attackComp->SetNumRebound(1);
			}
		},
		[](Character& obj) {
			if (const auto attackComp = obj.GetComponent<AttackComponent>(ComponentType::ATTACK)) {
				attackComp->SetNumRebound(0);
			}
		});

	list.emplace_back(4, "近戰反彈", "", [](Character& obj) {
		if (const auto attackComp = obj.GetComponent<AttackComponent>(ComponentType::ATTACK)) {
				attackComp->SetReflectBullet(true);
			}
		},
		[](Character& obj) {
			if (const auto attackComp = obj.GetComponent<AttackComponent>(ComponentType::ATTACK)) {
				attackComp->SetReflectBullet(false);
			}
		});

	return list;
}