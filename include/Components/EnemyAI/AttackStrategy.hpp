//
// Created by tjx20 on 4/17/2025.
//

#ifndef ATTACKSTRATEGY_HPP
#define ATTACKSTRATEGY_HPP

class Character;
struct EnemyContext;

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IAttackStrategy {
public:
	virtual ~IAttackStrategy() = default;
	virtual void Update(const EnemyContext &ctx, const float deltaTime) = 0;

protected:
	float m_attackCooldown = 3.5f;
	float m_attackTimer = 0.0f;
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class MeleeAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override;
};

class GunAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override
	{

	}

};

class CollisionAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override
	{

	}

};

class NoAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override {}

};
#endif //ATTACKSTRATEGY_HPP
