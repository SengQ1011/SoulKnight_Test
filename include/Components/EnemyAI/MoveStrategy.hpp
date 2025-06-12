//
// Created by tjx20 on 4/17/2025.
//

#ifndef MOVESTRATEGY_HPP
#define MOVESTRATEGY_HPP

#include <glm/vec2.hpp>
#include <memory>
#include <vector>

#include "EnumTypes.hpp"

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
	bool m_mandatoryRest = false; // 强制休息
	float m_restTimer = 0.0f; // 休息時間計時器
	float m_moveTimer = 0; // 移動時間計時器
	float m_detectionRange = 150.0f;

	void changeToIdle(const EnemyContext &ctx, float minTime, float maxTime);
	void ReflectMovement(const CollisionEventInfo &info, const EnemyContext &ctx);
	void EnterWanderState(const EnemyContext &ctx, float minTime, float maxTime, float moveRatio = 0.2f);
	void RestIfNeeded(float deltaTime, const EnemyContext &ctx, float minTime, float maxTime);

	// 移動邏輯
	void MaintainDistanceMove(const EnemyContext& ctx, float optimalDistance, float speed, bool faceToTarget);
	void MaintainCircleMove(const EnemyContext& ctx, float circleRadius, float speed);
	void MaintainStrafeMove(const EnemyContext& ctx, float optimalDistance, float deltaTime, float speed);
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
	void MaintainOptimalRangeForGun(const EnemyContext &ctx, std::shared_ptr<nGameObject> target, const std::shared_ptr<IAttackStrategy>& gunStrategy) const;
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

class BossMove final : public IMoveStrategy
{
public:
	void Update(const EnemyContext& ctx, float deltaTime) override;
	void UpdateSkillState(const EnemyContext &ctx, float deltaTime);

private:
	float m_skillTimer = 0.0f;  // 技能计时器
	float stateTimer = 0.0f;
	glm::vec2 m_jumpTargetPos;
	bool m_hasJumpTarget = false;
	std::vector<enemyState> m_skillQueue;

	void refillSkillQueue();


	void UpdateSkill1MoveState(const EnemyContext& ctx, float deltaTime);
	void UpdateSkill2MoveState(const EnemyContext& ctx, float deltaTime);
	void UpdateSkill3MoveState(const EnemyContext& ctx, float deltaTime);
	void UpdateSkill4MoveState(const EnemyContext& ctx, float deltaTime);
	void UpdateSkill5MoveState(const EnemyContext& ctx, float deltaTime);
};

#endif //MOVESTRATEGY_HPP
