//
// Created by tjx20 on 4/17/2025.
//

#include "Components/AiComponent.hpp"

#include "Components/AttackComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/EnemyAI/MoveStrategy.hpp"
#include "Components/EnemyAI/UtilityStrategy.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Creature/Character.hpp"
#include "Util/Time.hpp"


AIComponent::AIComponent(const MonsterType MonsterType, const std::shared_ptr<IMoveStrategy> &moveStrategy,
						 const std::unordered_map<AttackType, std::shared_ptr<IAttackStrategy>> &attackStrategies,
						 const std::shared_ptr<IUtilityStrategy> &utilityStrategies, const int monsterPoint) :
	Component(ComponentType::AI), m_aiType(MonsterType), m_moveStrategy(moveStrategy),
	m_attackStrategy(attackStrategies), m_utilityStrategy(utilityStrategies), m_monsterPoint(monsterPoint),
	m_enemyState(enemyState::IDLE)
{
}

void AIComponent::Init()
{
	const auto enemy = GetOwner<Character>();
	m_context.enemy = enemy;
	m_context.moveComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	m_context.stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);
	m_context.attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK);
}


void AIComponent::Update()
{
	float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;

	if (m_moveStrategy)
		m_moveStrategy->Update(m_context, deltaTime);
	if (m_utilityStrategy)
		m_utilityStrategy->Update(m_context);
}

void AIComponent::SetEnemyState(const enemyState state) const
{
	const auto enemy = GetOwner<Character>();
	if (!enemy)
		return;
	auto stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);
	if (state == enemyState::IDLE)
		stateComp->SetState(State::STANDING);
	else if (state == enemyState::WANDERING)
		stateComp->SetState(State::MOVING);
	else if (state == enemyState::CHASING)
		stateComp->SetState(State::MOVING);
	else if (state == enemyState::READY_ATTACK)
		stateComp->SetState(State::ATTACK);
}

void AIComponent::HandleCollision(CollisionInfo &info)
{
	// 攻擊后强制進入巡邏模式
	if (const auto character = std::dynamic_pointer_cast<Character>(info.GetObjectB()))
	{
		if (character->GetType() == CharacterType::PLAYER)
		{
			// m_enemyState = enemyState::IDLE;
			//  m_attackTimer = m_attackCooldown;
		}
	}

	// 碰到地形回轉
	if (const auto type =
			info.GetObjectB()->GetComponent<CollisionComponent>(ComponentType::COLLISION)->GetCollisionLayer();
		type == CollisionLayers_Terrain)
	{
		const auto enemy = GetOwner<Character>();
		const auto movementComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);

		glm::vec2 oldDir = glm::normalize(movementComp->GetLastValidDirection());
		glm::vec2 reflectDir = glm::reflect(oldDir, info.GetCollisionNormal());

		// 如果新方向與原方向夾角太小（例如反射結果幾乎沒有改變），強制稍微偏移一下
		if (glm::dot(oldDir, reflectDir) < -0.99f)
		{ // 夾角約179度
			glm::vec2 tangent = glm::vec2(-info.GetCollisionNormal().y, info.GetCollisionNormal().x); // 法線的垂直方向
			reflectDir += tangent * 0.2f; // 稍微偏一點
			reflectDir = glm::normalize(reflectDir);
		}
		movementComp->SetDesiredDirection(reflectDir);
	}
}

void AIComponent::OnPlayerPositionUpdate(std::weak_ptr<Character> player) { m_Target = player; }

