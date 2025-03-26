//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Character.hpp"


#ifndef ENEMY_HPP
#define ENEMY_HPP
class Enemy : public Character {
public:
	Enemy(int maxHp, float moveSpeed, std::unique_ptr<CollisionBox> collisionBox);
	~Enemy()override {};

	void Start() override;

private:

};

#endif //ENEMY_HPP
