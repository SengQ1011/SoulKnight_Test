//
// Created by tjx20 on 4/17/2025.
//

#include "Components/AiComponent.hpp"

#include "Components/AttackComponent.hpp"
#include "Components/EnemyAI/AttackStrategy.hpp"
#include "Components/EnemyAI/MoveStrategy.hpp"
#include "Components/EnemyAI/UtilityStrategy.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Image.hpp"
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
	m_readyAttackTimer = m_readyAttackTime;
	const auto enemy = GetOwner<Character>();

	std::string imagePath = RESOURCE_DIR"/icon/double-exclamation-mark.png";
	m_readyAttackIcon = std::make_shared<nGameObject>();
	m_readyAttackIcon->SetDrawable(std::make_shared<Util::Image>(imagePath));
	m_readyAttackIcon->SetZIndexType(ZIndexType::UI);
	m_readyAttackIcon->m_WorldCoord = GetOwner<Character>()->GetWorldCoord() + m_iconOffset;
	// enemy->AddChild(m_readyAttackIcon);
	HideReadyAttackIcon();
	const auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetRoot().lock()->AddChild(m_readyAttackIcon);
	scene->GetCamera().lock()->AddChild(m_readyAttackIcon);

	m_context.enemy = enemy;
	m_context.moveComp = enemy->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	m_context.attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK);
	m_context.stateComp = enemy->GetComponent<StateComponent>(ComponentType::STATE);

	// if (auto attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
	// 	if(auto currentWeapon = attackComp->GetCurrentWeapon()) {
	// 		// auto type = currentWeapon->GetWeaponType();
	// 	}
	// }

}


void AIComponent::Update() {
	const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	m_readyAttackIcon->m_WorldCoord = GetOwner<Character>()->GetWorldCoord() + m_iconOffset;
	if (m_moveStrategy)
		m_moveStrategy->Update(m_context, deltaTime);

	if (!m_attackStrategy.empty())
		for(const auto& pair : m_attackStrategy) {
			pair.second->Update(m_context, deltaTime);
		}
	if (m_utilityStrategy)
		m_utilityStrategy->Update(m_context);
}

void AIComponent::SetEnemyState(const enemyState state)
{
	m_enemyState = state;
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

void AIComponent::ShowReadyAttackIcon() const {
	m_readyAttackIcon->SetVisible(true);
}

void AIComponent::HideReadyAttackIcon()
{
	m_readyAttackIcon->SetVisible(false);
}

void AIComponent::OnPlayerPositionUpdate(std::weak_ptr<Character> player) { m_Target = player; }

std::vector<EventType> AIComponent::SubscribedEventTypes() const
{
	return {
		EventType::Collision
	};
}

// void AIComponent::HandleCollision(CollisionInfo &info)
// {
//	m_moveStrategy->CollisionAction(info, m_context);
// }

void AIComponent::HandleEvent(const EventInfo &eventInfo)
{
	const auto& collisionInfo = dynamic_cast<const CollisionEventInfo&>(eventInfo);
	m_moveStrategy->CollisionAction(collisionInfo, m_context);
}
