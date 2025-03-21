//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Enemy.hpp"

Enemy::Enemy(const std::unordered_map<State, std::shared_ptr<Animation>> animation, int maxHp, float speed, int aimRange, std::unique_ptr<CollisionBox> collisionBox, std::shared_ptr<Weapon>  initialWeapon)
	: Character(animation, maxHp, speed, aimRange, std::move(collisionBox), initialWeapon)  {}

void Enemy::Start()
{

}

void Enemy::Update(float deltaTime)
{

}

void Enemy::attack() {  }


void Enemy::move(const glm::vec2 movement) {  }
