//
// Created by tjx20 on 4/17/2025.
//

#ifndef MOVESTRATEGY_HPP
#define MOVESTRATEGY_HPP

#include <memory>

class IAttackStrategy;
struct CollisionEventInfo;
struct EnemyContext;
class nGameObject;

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IMoveStrategy
{
public:
	virtual ~IMoveStrategy() = default;
	virtual void Update(const EnemyContext &ctx, float deltaTime) = 0;
	void CollisionAction(const CollisionEventInfo &info, const EnemyContext &ctx);

protected:
	bool m_mandatoryRest = false;
	float m_restTimer = 0.0f; // 休息時間計時器
	float m_moveTimer = 0; // 移動時間計時器
	float m_detectionRange = 250.0f;

	void changeToIdle(const EnemyContext &ctx);
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class WanderMove final : public IMoveStrategy
{
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override;
};

class ChaseMove final : public IMoveStrategy
{
public:
	void Update(const EnemyContext &ctx, float deltaTime) override;
	void ChasePlayerLogic(const EnemyContext &ctx, std::shared_ptr<nGameObject> target) const;
	void MaintainOptimalRangeForGun(const EnemyContext &ctx, std::shared_ptr<nGameObject> target, std::shared_ptr<IAttackStrategy> gunStrategy) const;
	void checkAttackCondition(const EnemyContext &ctx) const;

private:
	const float m_meleeAttackDistance = 50.0f;
	const float m_gunAttackDistance = 250.0f;
};

class NoMove final : public IMoveStrategy
{
public:
	// Do nothing
	void Update(const EnemyContext &ctx, float deltaTime) override {}
};


#endif //MOVESTRATEGY_HPP
