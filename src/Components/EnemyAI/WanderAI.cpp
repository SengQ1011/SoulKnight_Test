//
// Created by tjx20 on 4/6/2025.
//

#include "Components/EnemyAI/WanderAI.hpp"

WanderAI::WanderAI(int monsterPoint): AIComponent(AIType::WANDER, monsterPoint) {}

void WanderAI::Update() {
	// 關於移動的deltaTime單位為MS
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	const auto enemy = GetOwner<Character>();
	if(!enemy)return;

	if (auto target = m_Target.lock()) {
		float distance = glm::distance(target->GetWorldCoord(), enemy->GetWorldCoord());
		if (distance < m_detectionRange) {
			ChasePlayerLogic();
			//m_enemyState = enemyState::CHASING;
			return;
		}
	}

	// 沒有追蹤目標，就進入 wander 模式
	WanderLogic(deltaTime);

	// switch (m_enemyState) {
	// 	case enemyState::IDLE:
	// 		break;
	// 	case enemyState::WANDERING:
	// 		break;
	// 	case enemyState::CHASING:
	// 		ChasePlayerLogic();
	// 		break;
	// 	case enemyState::READY_ATTACK:
	// 		break;
	// }
}

void WanderAI::WanderLogic(const float deltaTime) {
	auto enemy = GetOwner<Character>();
	auto movementComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	auto stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);

	if (!movementComp || !stateComp) return;

	m_wanderCooldown -= deltaTime; // 更新冷卻時間
	if (m_wanderCooldown <= 0) { // 如果冷卻時間結束
		m_wanderCooldown = RandomFloatInRange(1.0f, 3.5f); // 隨機設置下一次漫遊的冷卻時間
		m_wanderDirection = RandomDirectionInsideUnitCircle(); // 隨機生成一個漫遊方向
	}

	// Normalize 避免怪物以更快的速度沿斜線移動
	const float ratio = 0.2f; // 調整移動比例
	const glm::vec2 deltaDisplacement = glm::normalize(m_wanderDirection) * ratio;
	// 設置目標移動方向
	movementComp->SetDesiredDirection(deltaDisplacement);

	// 如果移動方向變化，反轉怪物的面朝方向
	if (deltaDisplacement.x < 0 && enemy->m_Transform.scale.x > 0 ||
		deltaDisplacement.x > 0 && enemy->m_Transform.scale.x < 0) {
		enemy->m_Transform.scale.x *= -1.0f; // 反轉X軸方向
		}

	// 設置狀態為移動
	stateComp->SetState(State::MOVING);
}

void WanderAI::ChasePlayerLogic() const{
	auto enemy = GetOwner<Character>();
	auto target = m_Target.lock();
	auto movementComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	auto stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);
	if (!movementComp || !stateComp) return;

	const float ratio = 0.2f; // 調整移動比例
	glm::vec2 dir = glm::normalize(target->GetWorldCoord() - enemy->GetWorldCoord())* ratio;
	movementComp->SetDesiredDirection(dir);
	// 反轉敵人的面朝方向
	if (dir.x < 0 && enemy->m_Transform.scale.x > 0 ||
		dir.x > 0 && enemy->m_Transform.scale.x < 0) {
		enemy->m_Transform.scale.x *= -1.0f; // 反轉X軸方向
		}
	stateComp->SetState(State::MOVING);
}

