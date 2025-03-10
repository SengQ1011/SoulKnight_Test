//
// Created by tjx20 on 3/10/2025.
//

#ifndef WEAPONFACTORY_HPP
#define WEAPONFACTORY_HPP

#include "Weapon/Weapon.hpp"
#include <memory>
#include <string>
#include <random>

class WeaponFactory {
public:
	// 根據給定的名稱創建武器
	std::unique_ptr<Weapon> createWeapon(const std::string& weaponName);

	// 隨機創建一個武器
	std::unique_ptr<Weapon> createRandomWeapon();

private:
	std::unique_ptr<Weapon> generateRandomSword();
	std::unique_ptr<Weapon> generateRandomBow();
};

#endif //WEAPONFACTORY_HPP
