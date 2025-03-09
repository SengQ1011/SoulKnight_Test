//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Enemy.hpp"

Enemy::Enemy(const std::string& ImagePath, int maxHp, float speed, int aimRange, CollisionBox* radius, Weapon* initialWeapon)
	: Character(ImagePath, maxHp, speed, aimRange, radius, initialWeapon) {}