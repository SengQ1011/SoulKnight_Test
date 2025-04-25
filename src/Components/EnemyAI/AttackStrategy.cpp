//
// Created by tjx20 on 4/25/2025.
//

#include "Components/EnemyAI/AttackStrategy.hpp"

#include "Components/AiComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "StructType.hpp"

void MeleeAttack::Update(const EnemyContext &ctx, const float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	if (aiComp->GetReadyAttackTimer() <= 0.0f)
	{
		aiComp->HideReadyAttackIcon();
		ctx.attackComp->TryAttack();
		aiComp->ResetReadyAttackTimer();
	}
}
