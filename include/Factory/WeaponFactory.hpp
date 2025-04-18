//
// Created by tjx20 on 3/10/2025.
//
// WeaponFactory.hpp
#ifndef WEAPONFACTORY_HPP
#define WEAPONFACTORY_HPP

#include <random>
#include "Factory/Factory.hpp"
#include "Weapon/GunWeapon.hpp"
#include "Weapon/MeleeWeapon.hpp"
#include "Weapon/Weapon.hpp"

namespace WeaponFactory {
	// 從 JSON 創建武器（直接使用 Factory 的 readJsonFile）
	std::shared_ptr<Weapon> createWeapon(int weaponID);
}

#endif //WEAPONFACTORY_HPP
