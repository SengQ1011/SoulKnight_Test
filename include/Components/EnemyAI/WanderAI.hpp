//
// Created by tjx20 on 4/6/2025.
//

#ifndef WANDERAI_HPP
#define WANDERAI_HPP

#include "pch.hpp"
struct CollisionInfo;

class WanderAI
{
public:
	explicit WanderAI(int monsterPoint);
	~WanderAI() = default;

	void Update();
	void HandleCollision(CollisionInfo &info);

private:
	void WanderLogic(float deltaTime);
	void ChasePlayerLogic() const;
	void ExecuteAttack();

	float m_attackCooldown = 2.0f;
	float m_attackTimer = 0;
	glm::vec2 m_wanderDirection = glm::vec2(0, 0);
	float m_wanderCooldown = 0;
	float m_detectionRange = 200.0f;
};

#endif //WANDERAI_HPP
