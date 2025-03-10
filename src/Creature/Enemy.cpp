//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Enemy.hpp"

Enemy::Enemy(const std::string& ImagePath, int maxHp, float speed, int aimRange, CollisionBox* radius, std::unique_ptr<Weapon>  initialWeapon)
	: Character(ImagePath, maxHp, speed, aimRange, radius, std::move(initialWeapon))  {}

void Enemy::Start()
{

}

void Enemy::Update()
{

}

void Enemy::attack() {  }


void Enemy::move(const glm::vec2 movement) {  }
