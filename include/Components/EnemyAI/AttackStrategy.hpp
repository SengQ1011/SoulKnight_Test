//
// Created by tjx20 on 4/17/2025.
//

#ifndef ATTACKSTRATEGY_HPP
#define ATTACKSTRATEGY_HPP

class Character;

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IAttackStrategy {
public:
	virtual ~IAttackStrategy() = default;
	virtual void Update(const EnemyContext &ctx, const float deltaTime) = 0;
	virtual void TryAttack(Character& owner) = 0;

private:
	float m_attackCooldown = 3.5f;
	float m_attackTimer = 0.0f;
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class MeleeAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override
	{

	}
	void TryAttack(Character& owner) override {

	}
};

class GunAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override
	{

	}
	void TryAttack(Character& owner) override {

	}
};

class CollisionAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override
	{

	}
	void TryAttack(Character& owner) override {

	}
};

class NoAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override {}
	// No attack
	void TryAttack(Character& owner) override {}
};
#endif //ATTACKSTRATEGY_HPP
