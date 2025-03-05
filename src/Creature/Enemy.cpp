//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Enemy.hpp"

Enemy::Enemy(const std::string& ImagePath, int maxHp, int currentHp, float speed, int aimRange, double radius, Weapon* initialWeapon)
	: Character(ImagePath, maxHp, currentHp,speed, aimRange, radius, initialWeapon) {}