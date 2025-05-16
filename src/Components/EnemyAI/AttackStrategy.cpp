//
// Created by tjx20 on 4/25/2025.
//

#include "Components/EnemyAI/AttackStrategy.hpp"

#include "Components/AiComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "StructType.hpp"

bool MeleeAttack::CanAttack(const EnemyContext &ctx) const
{
	auto aiComp = ctx.GetAIComp();
	if (const auto target = aiComp->GetTarget().lock(); target != nullptr)
	{
		const auto distance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		return distance < m_meleeAttackDistance;
	}
	return false;
}

void MeleeAttack::Update(const EnemyContext &ctx, const float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	if (aiComp->GetReadyAttackTimer() <= 0.0f)
	{
		aiComp->HideReadyAttackIcon();
		ctx.attackComp->TryAttack();
		aiComp->SetEnemyState(enemyState::IDLE);
		ctx.moveComp->SetDesiredDirection(glm::vec2{0.0f, 0.0f});
		aiComp->ResetReadyAttackTimer();
	}
}

bool GunAttack::CanAttack(const EnemyContext &ctx) const
{
	auto aiComp = ctx.GetAIComp();
	if (const auto target = aiComp->GetTarget().lock(); target != nullptr)
	{
		const auto distance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		return distance < m_gunAttackDistance && distance > m_gunAttackDistance * 0.3f;
	}
	return false;
}

void GunAttack::Update(const EnemyContext &ctx, const float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	if (aiComp->GetReadyAttackTimer() <= 0.0f)
	{
		aiComp->HideReadyAttackIcon();
		ctx.attackComp->TryAttack();
		aiComp->SetEnemyState(enemyState::IDLE);
		aiComp->ResetReadyAttackTimer();
	}
}

bool CollisionAttack::CanAttack(const EnemyContext &ctx) const
{
	return false;
}