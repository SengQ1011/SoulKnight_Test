//
// Created by tjx20 on 3/10/2025.
//
// WeaponFactory.hpp
#ifndef WEAPONFACTORY_HPP
#define WEAPONFACTORY_HPP

#include "Weapon/Weapon.hpp"

namespace WeaponFactory {
	// 從 JSON 創建武器（直接使用 Factory 的 readJsonFile）
	std::shared_ptr<Weapon> createWeapon(int weaponID);
	std::shared_ptr<Weapon> createRandomWeapon(std::vector<int> allPlayerWeaponID);
}

#endif //WEAPONFACTORY_HPP
