//
// Created by tjx20 on 3/10/2025.
//

#include "Weapon/WeaponFactory.hpp"
#include "Weapon/Weapon.hpp"
#include <random>
#include <iostream>

std::unique_ptr<Weapon> WeaponFactory::createWeapon(const std::string& weaponName) {

	return nullptr;
}

std::unique_ptr<Weapon> WeaponFactory::createRandomWeapon() {
	return nullptr;
}

std::unique_ptr<Weapon> WeaponFactory::generateRandomSword() {
	return nullptr;
}

std::unique_ptr<Weapon> WeaponFactory::generateRandomBow() {
	return nullptr;
}
