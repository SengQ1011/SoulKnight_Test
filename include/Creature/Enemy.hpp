//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Character.hpp"


#ifndef ENEMY_HPP
#define ENEMY_HPP
class Enemy : public Character {
public:
	Enemy(const std::string& ImagePath, int maxHp, float moveSpeed, int aimRange, CollisionBox* collisionRadius, Weapon* initialWeapon);
	~Enemy()override {};

private:

};

#endif //ENEMY_HPP
