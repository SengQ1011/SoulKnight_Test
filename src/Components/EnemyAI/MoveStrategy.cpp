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
#include "RandomUtil.hpp"

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
				m_restTimer = RandomUtil::RandomFloatInRange(0.3f, 1.0f); // 設定休息時間
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
				constexpr float ratio = 0.2f;
				const glm::vec2 deltaDisplacement = glm::normalize(RandomUtil::RandomDirectionInsideUnitCircle()) * ratio;
				ctx.moveComp->SetDesiredDirection(deltaDisplacement);
				// 設定移動持續時間
				m_moveTimer = RandomUtil::RandomFloatInRange(2.0f, 6.5f);
				aiComp->SetEnemyState(enemyState::WANDERING);
			}
			break;
		case enemyState::WANDERING:
			// 移動狀態
			m_moveTimer -= deltaTime;
			if (m_moveTimer <= 0) {
				// 移動結束，開始休息
				ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0)); // 停止移動
				m_restTimer = RandomUtil::RandomFloatInRange(1.0f, 3.0f); // 設定休息時間
				aiComp->SetEnemyState(enemyState::IDLE);
			}
			break;
		default:
			break;
	}
}


//============================= (ChaseMove) =============================
void checkHasTarget(const EnemyContext &ctx) {
	auto AiComp = ctx.GetAIComp();
	if (const auto target = AiComp->GetTarget().lock(); target != nullptr)
	{
		AiComp->SetEnemyState(enemyState::CHASING);
	}
}

void IMoveStrategy::changeToIdle(const EnemyContext &ctx) {
	const auto aiComp = ctx.GetAIComp();
	// 移動結束，開始休息
	ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0)); // 停止移動
	m_restTimer = RandomUtil::RandomFloatInRange(1.0f, 3.0f); // 設定休息時間
	aiComp->SetEnemyState(enemyState::IDLE);
}

void ChaseMove::Update(const EnemyContext &ctx, const float deltaTime) {
	auto aiComp = ctx.GetAIComp();

	switch (auto State = ctx.GetAIComp()->GetEnemyState()) {
		case enemyState::IDLE:
			m_restTimer -= deltaTime;
			if(!m_mandatoryRest) {
				checkHasTarget(ctx);
				m_mandatoryRest = false;
			}
			if (m_restTimer <= 0) {
				constexpr float ratio = 0.2f;
				const glm::vec2 deltaDisplacement = glm::normalize(RandomUtil::RandomDirectionInsideUnitCircle()) * ratio;
				ctx.moveComp->SetDesiredDirection(deltaDisplacement);
				m_moveTimer = RandomUtil::RandomFloatInRange(5.0f, 10.0f);
				aiComp->SetEnemyState(enemyState::WANDERING);
			}
			break;

		case enemyState::WANDERING:
			// 移動狀態
			m_moveTimer -= deltaTime;
			checkHasTarget(ctx);
			if (m_moveTimer <= 0) {
				changeToIdle(ctx);
			}
			break;
		case enemyState::CHASING:
			if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr){
				if (const auto gun = aiComp->GetAttackStrategy(AttackType::PROJECTILE)) {
					// 處理遠程武器的最佳距離保持
					MaintainOptimalRangeForGun(ctx, target, gun);
				} else {
					// 純近戰敵人正常追逐玩家
					ChasePlayerLogic(ctx, target);
				}

				checkAttackCondition(ctx);
			} else changeToIdle(ctx);
			break;

		case enemyState::READY_ATTACK:
			aiComp->DeductionReadyAttackTimer(deltaTime);
			if (aiComp->GetReadyAttackTimer() <= 0) {
				m_mandatoryRest = true;
				m_restTimer = RandomUtil::RandomFloatInRange(1.5f, 3.5f);
			} else {
				// 根據攻擊型別判讀停止
				if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
					if (const auto attack = aiComp->GetAttackStrategy(AttackType::EFFECT_ATTACK)) {
						// 繼續追蹤玩家
						ChasePlayerLogic(ctx, target);
					}
					else if (const auto gun = aiComp->GetAttackStrategy(AttackType::PROJECTILE)) {
						// 停止並瞄準
						ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0));
						//MaintainOptimalRangeForGun(ctx, target, gun);
					}
				}else {
					ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0));
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


void ChaseMove::MaintainOptimalRangeForGun(const EnemyContext &ctx, std::shared_ptr<nGameObject> target,
										  std::shared_ptr<IAttackStrategy> gunStrategy) const
{
	const float optimalDistance = gunStrategy->GetAttackDistance() * 0.8f; // 80% 的最大射程是最佳射击距离
	const float minDistance = gunStrategy->GetAttackDistance() * 0.4f;     // 最小保持距离

	const glm::vec2 enemyPosition = ctx.enemy->GetWorldCoord();
	const glm::vec2 targetPosition = target->GetWorldCoord();
	const float currentDistance = glm::distance(enemyPosition, targetPosition);

	glm::vec2 directionVector = glm::vec2(0, 0);

	if (currentDistance < minDistance) {
		// 太近了，需要后退
		directionVector = glm::normalize(enemyPosition - targetPosition) * 0.3f; // 后退速度稍快
	} else if (currentDistance < optimalDistance * 0.9f) {
		// 接近最佳距离但仍然有点近，慢慢后退
		directionVector = glm::normalize(enemyPosition - targetPosition) * 0.15f;
	} else if (currentDistance > optimalDistance * 1.1f) {
		// 太远了，需要接近
		directionVector = glm::normalize(targetPosition - enemyPosition) * 0.2f;
	} else {
		// 在最佳射击范围内，只需停下来瞄准
		directionVector = glm::vec2(0, 0);
	}

	ctx.moveComp->SetDesiredDirection(directionVector);
}

void ChaseMove::checkAttackCondition(const EnemyContext &ctx) const
{
	auto aiComp = ctx.GetAIComp();

	// 检查所有攻击类型，避免重复代码
	const std::array<AttackType, 2> attackTypes = {AttackType::EFFECT_ATTACK, AttackType::PROJECTILE};

	for (const auto &attackType : attackTypes) {
		if (const auto strategy = aiComp->GetAttackStrategy(attackType)) {
			if (strategy->CanAttack(ctx)) {
				aiComp->ShowReadyAttackIcon();
				aiComp->SetEnemyState(enemyState::READY_ATTACK);
				return;
			}
		}
	}
}