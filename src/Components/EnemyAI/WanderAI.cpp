//
// Created by tjx20 on 4/6/2025.
//

#include "Components/EnemyAI/WanderAI.hpp"

WanderAI::WanderAI(int monsterPoint) {}

void WanderAI::Update()
{
	// // 關於移動的deltaTime單位為MS
	// const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	// //const auto enemy = GetOwner<Character>();
	// if(!enemy)return;
	//
	// if (m_attackTimer > 0) {
	// 	// 繼續巡邏
	// 	m_attackTimer -= deltaTime;
	// }
	// //else if (const auto target = m_Target.lock()) {
	// 	float distance = glm::distance(target->GetWorldCoord(), enemy->GetWorldCoord());
	// 	if (distance < m_detectionRange) {
	// 		m_enemyState = enemyState::CHASING;
	// 	}
	// } // 沒有追蹤目標，就進入 wander 模式
	// else m_enemyState = enemyState::WANDERING;
	//
	// switch (m_enemyState) {
	// 	case enemyState::IDLE:
	// 		break;
	// 	case enemyState::WANDERING:
	// 		WanderLogic(deltaTime);
	// 		break;
	// 	case enemyState::CHASING:
	// 		ChasePlayerLogic();
	// 		break;
	// 	case enemyState::READY_ATTACK:
	// 		break;
	// 	default:
	// 		break;
	// }
}

void WanderAI::WanderLogic(const float deltaTime)
{
	// const auto enemy = GetOwner<Character>();
	// auto movementComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	// const auto stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);
	//
	// if (!movementComp || !stateComp) return;
	//
	// m_wanderCooldown -= deltaTime; // 更新冷卻時間
	// if (m_wanderCooldown <= 0) { // 如果冷卻時間結束
	// 	m_wanderCooldown = RandomFloatInRange(1.0f, 4.5f); // 隨機設置下一次漫遊的冷卻時間
	// 	m_wanderDirection = RandomDirectionInsideUnitCircle(); // 隨機生成一個漫遊方向
	// 	// Normalize 避免怪物以更快的速度沿斜線移動
	// 	constexpr float ratio = 0.2f; // 調整移動比例
	// 	const glm::vec2 deltaDisplacement = glm::normalize(m_wanderDirection) * ratio;
	// 	// 設置目標移動方向
	// 	movementComp->SetDesiredDirection(deltaDisplacement);
	//
	// 	// 如果移動方向變化，反轉怪物的面朝方向
	// 	if (deltaDisplacement.x < 0 && enemy->m_Transform.scale.x > 0 ||
	// 		deltaDisplacement.x > 0 && enemy->m_Transform.scale.x < 0) {
	// 		enemy->m_Transform.scale.x *= -1.0f; // 反轉X軸方向
	// 		}
	//
	// 	// 設置狀態為移動
	// 	stateComp->SetState(State::MOVING);
	// }
}

void WanderAI::ChasePlayerLogic() const
{
	// auto enemy = GetOwner<Character>();
	// auto target = m_Target.lock();
	// auto movementComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	// auto stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);
	// if (!movementComp || !stateComp) return;
	//
	// const float ratio = 0.2f; // 調整移動比例
	// glm::vec2 dir = glm::normalize(target->GetWorldCoord() - enemy->GetWorldCoord())* ratio;
	// movementComp->SetDesiredDirection(dir);
	// // 反轉敵人的面朝方向
	// if (dir.x < 0 && enemy->m_Transform.scale.x > 0 ||
	// 	dir.x > 0 && enemy->m_Transform.scale.x < 0) {
	// 	enemy->m_Transform.scale.x *= -1.0f; // 反轉X軸方向
	// 	}
	// stateComp->SetState(State::MOVING);
}

void WanderAI::HandleCollision(CollisionInfo &info)
{
	// // 攻擊后强制進入巡邏模式
	// if(const auto character = std::dynamic_pointer_cast<Character>(info.GetObjectB())){
	// 	if(character->GetType() == CharacterType::PLAYER) {
	// 		m_enemyState = enemyState::WANDERING;
	// 		m_attackTimer = m_attackCooldown;
	// 	}
	// }
	//
	// // 碰到地形回轉
	// if (const auto  type =
	// info.GetObjectB()->GetComponent<CollisionComponent>(ComponentType::COLLISION)->GetCollisionLayer(); 	type ==
	// CollisionLayers_Terrain)
	// {
	// 	const auto enemy = GetOwner<Character>();
	// 	const auto movementComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	//
	// 	glm::vec2 oldDir = glm::normalize(movementComp->GetLastValidDirection());
	// 	glm::vec2 reflectDir = glm::reflect(oldDir, info.GetCollisionNormal());
	//
	// 	// 如果新方向與原方向夾角太小（例如反射結果幾乎沒有改變），強制稍微偏移一下
	// 	if (glm::dot(oldDir, reflectDir) < -0.99f) { // 夾角約179度
	// 		glm::vec2 tangent = glm::vec2(-info.GetCollisionNormal().y, info.GetCollisionNormal().x); // 法線的垂直方向
	// 		reflectDir += tangent * 0.2f; // 稍微偏一點
	// 		reflectDir = glm::normalize(reflectDir);
	// 	}
	// 	movementComp->SetDesiredDirection(reflectDir);
	//
	// }
}
