//
// Created by tjx20 on 4/18/2025.
//
#include "Components/EnemyAI/MoveStrategy.hpp"
#include "Components/AiComponent.hpp"
#include "Components/EnemyAI/AttackStrategy.hpp"
#include "Components/MovementComponent.hpp"
#include "Creature/Character.hpp"
#include "Override/nGameObject.hpp"
#include "StructType.hpp"

//============================= (Base) =============================
void ReflectMovement(const CollisionEventInfo &info, const EnemyContext &ctx)
{
	glm::vec2 oldDir = glm::normalize(ctx.moveComp->GetLastValidDirection());
	glm::vec2 reflectDir = glm::reflect(oldDir, info.GetCollisionNormal());

	// 如果新方向與原方向夾角太小（例如反射結果幾乎沒有改變），強制稍微偏移一下
	if (glm::dot(oldDir, reflectDir) < -0.99f)
	{ // 夾角約179度
		glm::vec2 tangent = glm::vec2(-info.GetCollisionNormal().y, info.GetCollisionNormal().x); // 法線的垂直方向
		reflectDir += tangent * 0.2f; // 稍微偏一點
		reflectDir = glm::normalize(reflectDir);
	}
	ctx.moveComp->SetDesiredDirection(reflectDir);
}


void IMoveStrategy::CollisionAction(const CollisionEventInfo &info, const EnemyContext &ctx)
{
	// ememy與玩家碰撞后:
	// 攻擊模式：反彈繼續走
	// 其他模式：强制進入閑置狀態
	if (const auto character = std::dynamic_pointer_cast<Character>(info.GetObjectB()))
	{
		if (character->GetType() == CharacterType::PLAYER){
			auto aiComp = ctx.GetAIComp();
			if(aiComp->GetEnemyState() == enemyState::WANDERING || aiComp->GetEnemyState() == enemyState::READY_ATTACK) {
				ReflectMovement(info, ctx);
			} else {
				aiComp->SetEnemyState(enemyState::IDLE);
				ctx.moveComp->SetDesiredDirection(glm::vec2(0,0));
				m_restTimer = RandomFloatInRange(0.3f, 1.0f); // 設定休息時間
			}
		}
	}

	// 碰到地形回轉
	if (const auto type = info.GetObjectB()->GetComponent<CollisionComponent>(ComponentType::COLLISION)->GetCollisionLayer();
		type == CollisionLayers_Terrain)
		ReflectMovement(info, ctx);
}

//============================= (Wander) =============================
void WanderMove::Update(const EnemyContext &ctx, const float deltaTime)
{
	const auto aiComp = ctx.GetAIComp();
	switch (auto State = ctx.GetAIComp()->GetEnemyState()) {
		// 待機狀態
		case enemyState::IDLE:
			m_restTimer -= deltaTime;
			if (m_restTimer <= 0) {
				// 休息結束，開始移動
				m_wanderDirection = RandomDirectionInsideUnitCircle();
				constexpr float ratio = 0.2f;
				const glm::vec2 deltaDisplacement = glm::normalize(m_wanderDirection) * ratio;
				ctx.moveComp->SetDesiredDirection(deltaDisplacement);
				// 設定移動持續時間
				m_moveTimer = RandomFloatInRange(2.0f, 6.5f);
				aiComp->SetEnemyState(enemyState::WANDERING);
			}
			break;
		case enemyState::WANDERING:
			// 移動狀態
			m_moveTimer -= deltaTime;
			if (m_moveTimer <= 0) {
				// 移動結束，開始休息
				ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0)); // 停止移動
				m_restTimer = RandomFloatInRange(1.0f, 3.0f); // 設定休息時間
				aiComp->SetEnemyState(enemyState::IDLE);
			}
			break;
		default:
			break;
	}
}


//============================= (ChaseMove) =============================
void ChaseMove::Update(const EnemyContext &ctx, const float deltaTime)
{
	auto aiComp = ctx.GetAIComp();

	switch (auto State = ctx.GetAIComp()->GetEnemyState()) {
		case enemyState::IDLE:
			m_restTimer -= deltaTime;
			if (m_restTimer <= 0) {
				m_moveTimer = RandomFloatInRange(5.0f, 10.0f);
				aiComp->SetEnemyState(enemyState::CHASING);
			}
			break;

		case enemyState::CHASING:
			m_moveTimer -= deltaTime;
			if (m_moveTimer <= 0) {
				aiComp->SetEnemyState(enemyState::IDLE);
				m_restTimer = RandomFloatInRange(0.5f, 1.0f);
			} else {
				if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr){
					ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0));
					// LOG_DEBUG("chasing");
					ChasePlayerLogic(ctx, target);
					if (const auto distance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
						distance < m_attackDistance)
					{
						LOG_DEBUG("can attack");
						aiComp->ShowReadyAttackIcon();
						aiComp->SetEnemyState(enemyState::READY_ATTACK);
					}
				} else break;
			}

			break;

		case enemyState::READY_ATTACK:
			aiComp->DeductionReadyAttackTimer(deltaTime);
			if (aiComp->GetReadyAttackTimer() <= 0) {
				m_restTimer = RandomFloatInRange(0.7f, 2.0f);
				aiComp->SetEnemyState(enemyState::IDLE);
			} else {
				// 根據攻擊型別判讀停止
				if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
					if(const auto attack = aiComp->GetAttackStrategy(AttackType::MELEE)) {
						// 繼續追蹤玩家
						ChasePlayerLogic(ctx, target);
					}
					else if (const auto gun = aiComp->GetAttackStrategy(AttackType::GUN)) {
						//TODO
					}
				}else {
					// m_restTimer = RandomFloatInRange(0.3f, 1.0f);
					// aiComp->SetEnemyState(enemyState::IDLE);
				}
			}
			break;
		default:
			break;
	}
}


void ChaseMove::ChasePlayerLogic(const EnemyContext &ctx, std::shared_ptr<nGameObject> target) const
{
	const float ratio = 0.2f; // 調整移動比例
	glm::vec2 dir = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord()) * ratio;
	ctx.moveComp->SetDesiredDirection(dir);
}


