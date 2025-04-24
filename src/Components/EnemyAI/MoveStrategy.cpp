//
// Created by tjx20 on 4/18/2025.
//
#include "Components/EnemyAI/MoveStrategy.hpp"
#include "Components/AiComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Creature/Character.hpp"
#include "Override/nGameObject.hpp"

void WanderMove::Update(const EnemyContext &ctx, const float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	if (ctx.stateComp->GetCurrentState() == State::STANDING)
	{
		// 待機狀態
		m_restTimer -= deltaTime;
		if (m_restTimer <= 0)
		{
			// 休息結束，開始移動
			m_wanderDirection = RandomDirectionInsideUnitCircle();
			constexpr float ratio = 0.2f;
			const glm::vec2 deltaDisplacement = glm::normalize(m_wanderDirection) * ratio;
			ctx.moveComp->SetDesiredDirection(deltaDisplacement);
			// 設定移動持續時間
			m_wanderCooldown = RandomFloatInRange(2.0f, 6.5f);
			ctx.stateComp->SetState(State::MOVING);
		}
	}
	else if (ctx.stateComp->GetCurrentState() == State::MOVING)
	{
		// if(auto target = ctx.GetAIComp()->GetTarget().lock())
		// {
		// 	ChasePlayerLogic(ctx, target);
		// 	return;
		// }
		// 移動狀態
		m_wanderCooldown -= deltaTime;
		if (m_wanderCooldown <= 0)
		{
			// 移動結束，開始休息
			ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0)); // 停止移動
			m_restTimer = RandomFloatInRange(1.0f, 3.0f); // 設定休息時間
			ctx.stateComp->SetState(State::STANDING);
		}
	}
}


void ChaseMove::Update(const EnemyContext &ctx, const float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	if (auto target = ctx.GetAIComp()->GetTarget().lock())
	{
		ChasePlayerLogic(ctx, target);
	}
	else
	{
		ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0));
		ctx.stateComp->SetState(State::STANDING);
	}
}


void ChaseMove::ChasePlayerLogic(const EnemyContext &ctx, std::shared_ptr<nGameObject> target) const
{
	const float ratio = 0.2f; // 調整移動比例
	glm::vec2 dir = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord()) * ratio;
	ctx.moveComp->SetDesiredDirection(dir);
	ctx.stateComp->SetState(State::MOVING);
}


