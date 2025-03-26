//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Enemy.hpp"

Enemy::Enemy(int maxHp, float speed, std::unique_ptr<CollisionBox> collisionBox)
	: Character(maxHp, speed, std::move(collisionBox))  {}

void Enemy::Start()
{

}
