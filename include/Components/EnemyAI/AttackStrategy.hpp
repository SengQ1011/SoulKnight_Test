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

	[[nodiscard]] virtual bool CanAttack(const EnemyContext &ctx) const = 0;
	[[nodiscard]] virtual float GetAttackDistance() const = 0;

protected:
	float m_attackCooldown = 5.0f;
	float m_attackTimer = 0.0f;
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class MeleeAttack final : public IAttackStrategy
{
public:
	void Update(const EnemyContext &ctx, float deltaTime) override;
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) const override;
	[[nodiscard]] float GetAttackDistance() const override { return m_meleeAttackDistance; }

private:
	const float m_meleeAttackDistance = 50.0f;
};

class GunAttack final : public IAttackStrategy
{
public:
	void Update(const EnemyContext &ctx, float deltaTime) override;
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) const override;
	[[nodiscard]] float GetAttackDistance() const override { return m_gunAttackDistance; }

private:
	const float m_gunAttackDistance = 200.0f;
};

class CollisionAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override{}
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) const override;
	[[nodiscard]] float GetAttackDistance() const override { return 0; }
};

class NoAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override {}
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) const override{ return false; }
	[[nodiscard]] float GetAttackDistance() const override { return 0; }
};
#endif //ATTACKSTRATEGY_HPP