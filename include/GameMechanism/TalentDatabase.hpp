//
// Created by tjx20 on 4/5/2025.
//

#ifndef TALENTDATABASE_HPP
#define TALENTDATABASE_HPP


#include "GameMechanism/Talent.hpp"
#include "Creature/Character.hpp"
#include "Components/HealthComponent.hpp"
#include <vector>
#include <functional>

std::vector<Talent> CreateTalentList() {
	std::vector<Talent> list;

	list.emplace_back("破甲保護", "/UI/ui_talentIcon/ui_buff_18.png", [](Character& obj) {
			if (auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetBreakProtection(true);
			}
		},
		[](Character& obj) {
			if (auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetBreakProtection(false);
			}
		});

	list.emplace_back("護甲+1", "/UI/ui_talentIcon/ui_buff_28.png", [](Character& obj){
			if (auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetMaxArmor(healthComp->GetMaxArmor() + 1);
			}
		},
		[](Character& obj) {
			if (auto healthComp = obj.GetComponent<HealthComponent>(ComponentType::HEALTH)) {
				healthComp->SetMaxArmor(healthComp->GetMaxArmor() - 1);
			}
		});

	// list.emplace_back("近戰反彈", "", [](Character& obj) {
	// 	if (obj.HasComponent<ReflectComp>()) {
	// 		obj.GetComponent<ReflectComp>().EnableReflect(true);  // 啟用反彈
	// 	}
	// });

	return list;
}


#endif //TALENTDATABASE_HPP
