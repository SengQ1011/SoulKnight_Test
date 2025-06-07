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
#include "Util/Time.hpp"

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
	m_restTimer = RandomUtil::RandomFloatInRange(2.0f, 5.0f); // 設定休息時間
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
				if (const auto gun = aiComp->GetAttackStrategy(AttackStrategies::GUN)) {
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
					if (const auto attack = aiComp->GetAttackStrategy(AttackStrategies::MELEE)) {
						// 繼續追蹤玩家
						ChasePlayerLogic(ctx, target);
					}
					else if (const auto gun = aiComp->GetAttackStrategy(AttackStrategies::GUN)) {
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
										  const std::shared_ptr<IAttackStrategy>& gunStrategy) const
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
		ctx.stateComp->SetState(State::STANDING);
	}

	ctx.moveComp->SetDesiredDirection(directionVector);
}

void ChaseMove::checkAttackCondition(const EnemyContext &ctx) const
{
	const auto aiComp = ctx.GetAIComp();

	// 检查所有攻击类型，避免重复代码
	const std::array<AttackStrategies, 2> attackTypes = {AttackStrategies::MELEE, AttackStrategies::GUN};

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

void BossMove::Update(const EnemyContext &ctx, float deltaTime)
{
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
		m_moveTimer -= deltaTime;
		checkHasTarget(ctx);
		if (m_moveTimer <= 0) {
			changeToIdle(ctx);
		}
		break;

	case enemyState::CHASING:
		if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
			const float distance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
			LOG_DEBUG("set skillState");
			aiComp->SetEnemyState(enemyState::SKILL5);

			// // 根據距離選擇技能
			// if (distance > 200.0f) {
			// 	aiComp->SetEnemyState(enemyState::SKILL1); // 遠程攻擊
			// } else if (distance < 100.0f) {
			// 	aiComp->SetEnemyState(enemyState::SKILL4); // 近戰攻擊
			// } else {
			// 	// 隨機選擇其他技能
			// 	int randomSkill = RandomUtil::RandomIntInRange(1, 5);
			// 	aiComp->SetEnemyState(static_cast<enemyState>(static_cast<int>(enemyState::SKILL1) + randomSkill - 1));
			// }
		} else {
			changeToIdle(ctx);
		}
		break;

	case enemyState::SKILL1:
	case enemyState::SKILL2:
	case enemyState::SKILL3:
	case enemyState::SKILL4:
	case enemyState::SKILL5:
		UpdateSkillState(ctx, deltaTime);
		break;

	default:
		break;
	}
}

void BossMove::UpdateSkillState(const EnemyContext &ctx, float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	aiComp->DeductionSkillTimer(deltaTime);

	if (aiComp->GetSkillTimer() <= 0) {
		// 技能結束，回到休息狀態
		LOG_DEBUG("BossMove::end");
		m_mandatoryRest = true;
		changeToIdle(ctx);
	} else {
		// 根據不同技能執行不同的移動邏輯
		switch (aiComp->GetEnemyState()) {
		case enemyState::SKILL1:
			UpdateSkill1MoveState(ctx, deltaTime);
			break;
		case enemyState::SKILL2:
			UpdateSkill2MoveState(ctx, deltaTime);
			break;
		case enemyState::SKILL3:
			UpdateSkill3MoveState(ctx, deltaTime);
			break;
		case enemyState::SKILL4:
			UpdateSkill4MoveState(ctx, deltaTime);
			break;
		case enemyState::SKILL5:
			UpdateSkill5MoveState(ctx, deltaTime);
			break;
		default:
			break;
		}
	}
}

void BossMove::UpdateSkill1MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 技能1：遠程攻擊，保持距離
	if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
		const float optimalDistance = 150.0f;
		const float currentDistance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		
		if (currentDistance < optimalDistance * 0.8f) {
			// 太近了，後退
			glm::vec2 direction = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection(direction * 0.3f);
		} else if (currentDistance > optimalDistance * 1.2f) {
			// 太遠了，前進
			glm::vec2 direction = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection(direction * 0.2f);
		} else {
			// 在最佳距離，停止移動
			ctx.moveComp->SetDesiredDirection(glm::vec2(0.0f));
		}
	}
}

void BossMove::UpdateSkill2MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 技能2：環形彈幕，繞著玩家轉圈
	const auto aiComp = ctx.GetAIComp();
	if (const auto target = aiComp->GetTarget().lock(); target != nullptr) {
		const float circleRadius = 100.0f;
		const float currentDistance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		
		// 計算切線方向
		glm::vec2 toTarget = target->GetWorldCoord() - ctx.enemy->GetWorldCoord();
		glm::vec2 tangent = glm::vec2(-toTarget.y, toTarget.x);
		tangent = glm::normalize(tangent);
		
		// 調整距離
		if (currentDistance < circleRadius * 0.8f) {
			// 太近了，向外移動
			glm::vec2 outward = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection((outward + tangent) * 0.3f);
		} else if (currentDistance > circleRadius * 1.2f) {
			// 太遠了，向內移動
			glm::vec2 inward = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection((inward + tangent) * 0.3f);
		} else {
			// 在正確距離，繞圈
			ctx.moveComp->SetDesiredDirection(tangent * 0.3f);
		}
	}
	const auto boss =  aiComp->GetOwner<Character>();
	boss->m_Transform.scale.x = std::abs(boss->m_Transform.scale.x);
}

void BossMove::UpdateSkill3MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 技能3：追蹤彈幕，保持中等距離
	if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
		const float optimalDistance = 150.0f;
		const float currentDistance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		
		if (currentDistance < optimalDistance * 0.8f) {
			// 太近了，後退
			glm::vec2 direction = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection(direction * 0.3f);
		} else if (currentDistance > optimalDistance * 1.2f) {
			// 太遠了，前進
			glm::vec2 direction = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection(direction * 0.2f);
		} else {
			// 在最佳距離，停止移動
			ctx.moveComp->SetDesiredDirection(glm::vec2(0.0f));
		}
	}
}

void BossMove::UpdateSkill4MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 技能4：近戰範圍攻擊，追擊玩家
	if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
		const float attackDistance = 100.0f;
		const float currentDistance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		
		if (currentDistance > attackDistance) {
			// 追擊玩家
			glm::vec2 direction = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection(direction * 0.4f);
		} else {
			// 在攻擊範圍內，停止移動
			ctx.moveComp->SetDesiredDirection(glm::vec2(0.0f));
		}
	}
}

void BossMove::UpdateSkill5MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 技能5：多重散射，保持中等距離並左右移動
	if (const auto target = ctx.GetAIComp()->GetTarget().lock(); target != nullptr) {
		const float optimalDistance = 250.0f;
		const float currentDistance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		
		// 計算左右移動
		glm::vec2 toTarget = target->GetWorldCoord() - ctx.enemy->GetWorldCoord();
		glm::vec2 perpendicular = glm::vec2(-toTarget.y, toTarget.x);
		perpendicular = glm::normalize(perpendicular);
		
		// 使用累積的deltaTime來計算左右移動
		static float accumulatedTime = 0.0f;
		accumulatedTime += deltaTime;
		float sideMovement = sin(accumulatedTime * 2.0f) * 0.3f;
		
		if (currentDistance < optimalDistance * 0.8f) {
			// 太近了，後退
			glm::vec2 direction = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection((direction + perpendicular * sideMovement) * 0.3f);
		} else if (currentDistance > optimalDistance * 1.2f) {
			// 太遠了，前進
			glm::vec2 direction = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
			ctx.moveComp->SetDesiredDirection((direction + perpendicular * sideMovement) * 0.3f);
		} else {
			// 在最佳距離，只做左右移動
			ctx.moveComp->SetDesiredDirection(perpendicular * sideMovement);
		}
	}
}

