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
void IMoveStrategy::ReflectMovement(const CollisionEventInfo &info, const EnemyContext &ctx)
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
		type == CollisionLayers_Terrain || type == CollisionLayers_DestructibleTerrain)
		ReflectMovement(info, ctx);
}

void IMoveStrategy::EnterWanderState(const EnemyContext &ctx, float minTime, float maxTime, float moveRatio) {
	glm::vec2 deltaDir = glm::normalize(RandomUtil::RandomDirectionInsideUnitCircle()) * moveRatio;
	ctx.moveComp->SetDesiredDirection(deltaDir);
	// 設定移動持續時間
	m_moveTimer = RandomUtil::RandomFloatInRange(minTime, maxTime);
	ctx.GetAIComp()->SetEnemyState(enemyState::WANDERING);
}

void IMoveStrategy::RestIfNeeded(float deltaTime, const EnemyContext &ctx, float minTime, float maxTime) {
	m_restTimer -= deltaTime;
	if (m_restTimer <= 0.f) {
		EnterWanderState(ctx, minTime, maxTime);
	}
}

void IMoveStrategy::changeToIdle(const EnemyContext &ctx) {
	const auto aiComp = ctx.GetAIComp();
	// 移動結束，開始休息
	ctx.moveComp->SetDesiredDirection(glm::vec2(0, 0)); // 停止移動
	m_restTimer = RandomUtil::RandomFloatInRange(2.0f, 5.0f); // 設定休息時間
	aiComp->SetEnemyState(enemyState::IDLE);
}

void IMoveStrategy::MaintainDistanceMove(const EnemyContext& ctx, float optimalDistance, float speed, bool faceToTarget)
{
	// 維持距離移動
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	float currentDistance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
	glm::vec2 direction;

	if (currentDistance < optimalDistance * 0.8f) {
		// 太近了，後退
		direction = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
		ctx.moveComp->SetDesiredDirection(direction * speed);
	} else if (currentDistance > optimalDistance * 1.2f) {
		// 太遠了，前進
		direction = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
		ctx.moveComp->SetDesiredDirection(direction * speed);
	} else {
		// 在最佳距離，停止移動
		ctx.moveComp->SetDesiredDirection(glm::vec2(0.0f));
	}

	// 保持向著目標
	if (faceToTarget)
	{
		if (target->GetWorldCoord().x > ctx.enemy->GetWorldCoord().x)
			// 面向右邊
			ctx.enemy->m_Transform.scale.x = std::abs(ctx.enemy->m_Transform.scale.x);
		else
			ctx.enemy->m_Transform.scale.x = -std::abs(ctx.enemy->m_Transform.scale.x);
	}
}

void IMoveStrategy::MaintainCircleMove(const EnemyContext& ctx, float circleRadius, float speed)
{
	// 繞圈
	const auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	// 計算切線方向
	glm::vec2 toTarget = target->GetWorldCoord() - ctx.enemy->GetWorldCoord();
	glm::vec2 tangent = glm::normalize(glm::vec2(-toTarget.y, toTarget.x));
	float currentDistance = glm::length(toTarget);

	if (currentDistance < circleRadius * 0.8f) {
		// 太近了，向外移動
		glm::vec2 outward = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
		ctx.moveComp->SetDesiredDirection((outward + tangent) * speed);
	} else if (currentDistance > circleRadius * 1.2f) {
		// 太遠了，向內移動
		glm::vec2 inward = glm::normalize(toTarget);
		ctx.moveComp->SetDesiredDirection((inward + tangent) * speed);
	} else {
		// 在正確距離，繞圈
		ctx.moveComp->SetDesiredDirection(tangent * speed);
	}
}

void IMoveStrategy::MaintainStrafeMove(const EnemyContext& ctx, float optimalDistance, float deltaTime, float speed)
{
	// 左右擺動==>蛇行
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	// 計算左右移動
	glm::vec2 toTarget = target->GetWorldCoord() - ctx.enemy->GetWorldCoord();
	glm::vec2 perpendicular = glm::normalize(glm::vec2(-toTarget.y, toTarget.x));
	glm::vec2 direction;

	// 使用累積的deltaTime來計算左右移動
	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;
	float sideMovement = sin(accumulatedTime * 2.0f) * speed;

	float currentDistance = glm::length(toTarget);
	if (currentDistance < optimalDistance * 0.8f) {
		// 太近了，後退
		direction = glm::normalize(ctx.enemy->GetWorldCoord() - target->GetWorldCoord());
		ctx.moveComp->SetDesiredDirection((direction + perpendicular * sideMovement) * speed);
	} else if (currentDistance > optimalDistance * 1.2f) {
		// 太遠了，前進
		direction = glm::normalize(toTarget);
		ctx.moveComp->SetDesiredDirection((direction + perpendicular * sideMovement) * speed);
	} else {
		// 在最佳距離，只做左右移動
		ctx.moveComp->SetDesiredDirection(perpendicular * sideMovement);
	}
}

//============================= (Wander) =============================
void WanderMove::Update(const EnemyContext &ctx, const float deltaTime)
{
	const auto aiComp = ctx.GetAIComp();
	switch (auto State = ctx.GetAIComp()->GetEnemyState()) {
		// 待機狀態
		case enemyState::IDLE:
			RestIfNeeded(deltaTime, ctx, 2.5f, 6.0f);
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

void ChaseMove::Update(const EnemyContext &ctx, const float deltaTime) {
	auto aiComp = ctx.GetAIComp();

	switch (auto State = ctx.GetAIComp()->GetEnemyState()) {
		case enemyState::IDLE:
			RestIfNeeded(deltaTime, ctx, 5.0f, 10.0f);
			if(!m_mandatoryRest) {
				checkHasTarget(ctx);
				m_mandatoryRest = false;
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

	// 检查所有攻击类型
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

//============================= (BossMove) =============================
void BossMove::refillSkillQueue() {
	m_skillQueue.clear();
	for (int i = static_cast<int>(enemyState::SKILL1); i <= static_cast<int>(enemyState::SKILL5); ++i) {
		m_skillQueue.push_back(static_cast<enemyState>(i));
	}
	// 随机打乱
	RandomUtil::RandomShuffle(m_skillQueue);
}
void BossMove::Update(const EnemyContext &ctx, float deltaTime)
{
	auto aiComp = ctx.GetAIComp();
	switch (auto State = ctx.GetAIComp()->GetEnemyState()) {
	case enemyState::IDLE:
		RestIfNeeded(deltaTime, ctx, 5.0f, 10.0f);
		if(!m_mandatoryRest) {
			checkHasTarget(ctx);
			m_mandatoryRest = false;
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
			// 每次追擊觸發技能前，都從queue裡 pop
			if (m_skillQueue.empty()) {
				refillSkillQueue();  // 重新填充並打亂
			}
			enemyState next = m_skillQueue.front();
			m_skillQueue.erase(m_skillQueue.begin());
			aiComp->SetEnemyState(next);

			if (aiComp->GetEnemyState() == enemyState::SKILL4 && !m_hasJumpTarget) {
				if (const auto target = aiComp->GetTarget().lock(); target) {
					m_jumpTargetPos = target->GetWorldCoord();
					m_hasJumpTarget = true;
				}
			}
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
		m_mandatoryRest = true;
		m_hasJumpTarget = false;
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
	MaintainDistanceMove(ctx, 150.0f, 0.3f, true);
}

void BossMove::UpdateSkill2MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 繞著玩家轉圈
	MaintainCircleMove(ctx, 100.0f, 0.2f);
	ctx.enemy->m_Transform.scale.x = std::abs(ctx.enemy->m_Transform.scale.x);
}

void BossMove::UpdateSkill3MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 保持中等距離
	MaintainDistanceMove(ctx, 150.0f, 0.2f, true);
}

void BossMove::UpdateSkill4MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 技能4：跳向玩家位置
	const auto attackStrategy = ctx.GetAIComp()->GetAttackStrategy(AttackStrategies::BOSS);
	if(const auto& bossAttackStrategy = std::dynamic_pointer_cast<BossAttackStrategy>(attackStrategy))
	{
		auto& skill = bossAttackStrategy->GetCurrentSkillInfo();
		// 前搖 + 空中階段（跳躍中）：移動到目標位置
		if (skill.activeTimer < skill.castTime) {
			if (m_hasJumpTarget) {
				glm::vec2 toTarget = m_jumpTargetPos - ctx.enemy->GetWorldCoord();
				if (glm::length(toTarget) > 5.0f) {
					ctx.moveComp->SetDesiredDirection(glm::normalize(toTarget) * 0.5f);
					ctx.moveComp->SetCurrentSpeedRatio(ctx.moveComp->GetSpeedRatio() * 1.5f);
				} else {
					ctx.moveComp->SetDesiredDirection(glm::vec2(0.0f));
				}
			}
		}
		// 落地階段：保持不動
		else {
			ctx.moveComp->SetDesiredDirection(glm::vec2(0.0f));
		}

		// 面向跳躍方向
		if (m_hasJumpTarget) {
			ctx.enemy->m_Transform.scale.x = (m_jumpTargetPos.x > ctx.enemy->GetWorldCoord().x)
				? std::abs(ctx.enemy->m_Transform.scale.x)
				: -std::abs(ctx.enemy->m_Transform.scale.x);
		}
	}
	else
	{
		LOG_ERROR("BossMove::UpdateSkill4MoveState==>can't get strategy");
	}

}

void BossMove::UpdateSkill5MoveState(const EnemyContext &ctx, float deltaTime)
{
	// 左右移動
	MaintainStrafeMove(ctx, 80.0f, deltaTime, 0.2f);
}