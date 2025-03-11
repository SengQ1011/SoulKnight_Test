//
// Created by tjx20 on 3/4/2025.
//

#include "Creature/Character.hpp"


#ifndef ENEMY_HPP
#define ENEMY_HPP
class Enemy : public Character {
public:
	Enemy(const std::string& ImagePath, int maxHp, float moveSpeed, int aimRange, std::unique_ptr<CollisionBox> collisionBox, std::unique_ptr<Weapon>  initialWeapon);
	~Enemy()override {};

	void Start() override;
	void Update() override;

	void attack() override;
	void move(const glm::vec2 movement) override;


private:

};

#endif //ENEMY_HPP
