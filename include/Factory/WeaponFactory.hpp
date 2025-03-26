//
// Created by tjx20 on 3/10/2025.
//

#ifndef WEAPONFACTORY_HPP
#define WEAPONFACTORY_HPP

#include <random>
#include "Factory/Factory.hpp"
#include "Weapon/GunWeapon.hpp"
#include "Weapon/MeleeWeapon.hpp"
#include "Weapon/Weapon.hpp"

class WeaponFactory: public Factory {
public:
	WeaponFactory();
	~WeaponFactory() override = default;

	// 從 JSON 創建武器
	std::shared_ptr<Weapon> createWeapon(const int weaponID);

	// 隨機創建一個武器
	std::shared_ptr<Weapon> createRandomWeapon();
};

#endif //WEAPONFACTORY_HPP
