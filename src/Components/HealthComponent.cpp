//
// Created by tjx20 on 3/28/2025.
//

#include "Components/HealthComponent.hpp"

#include "Components/CollisionComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Override/nGameObject.hpp"
#include "Scene/SceneManager.hpp"
#include "Util/Time.hpp"
#include "Weapon/Bullet.hpp"

HealthComponent::HealthComponent(const int maxHp, const int maxArmor = 0, const int maxEnergy = 0)
	: Component(ComponentType::HEALTH),m_maxHp(maxHp), m_currentHp(maxHp),
		m_maxArmor(maxArmor), m_currentArmor(maxArmor),
		m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy) {}

void HealthComponent::Update() {
	const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	if(m_maxArmor == 0) return;
	if(m_currentArmor < m_maxArmor) {
		m_armorRecoveryTimer += deltaTime;
		if (m_armorRecoveryTimer >= m_armorRecoveryInterval) {
			AddCurrentArmor(1);
			m_armorRecoveryTimer = 0.0f;
		}
	}
}

void HealthComponent::HandleCollision(CollisionInfo &info){
	// 判斷碰撞對象是不是子彈==>因爲碰撞manager已經檢查是否為敵方子彈，所以不需要再判斷
	if (const auto bullet = std::dynamic_pointer_cast<Bullet>(info.GetObjectB())) {
		const int damage = bullet->GetDamage();
		this->TakeDamage(damage);
	}

	// collisionEnemy的碰撞傷害
	if(const auto character = std::dynamic_pointer_cast<Character>(info.GetObjectB())){
		if(character->GetType() == CharacterType::ENEMY) {
			if (const auto collisionDamage = character->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetCollisionDamage();
				collisionDamage > 0){
				this->TakeDamage(collisionDamage);
			}
		}
	}
}

void HealthComponent::TakeDamage(int damage) {
	// 天賦：破甲保護
	if (m_breakProtection && damage > m_currentArmor && m_currentArmor > 0) {
		m_currentArmor = 0;  // 只扣盔甲
		return;
	}
	int remainingDamage = std::max(0, damage - m_currentArmor);
	m_currentArmor = std::max(0, m_currentArmor - damage);
	m_currentHp = std::max(0, m_currentHp - remainingDamage);

	if (m_currentHp == 0) {
		OnDeath();
	}
}

void HealthComponent::OnDeath() const
{
	auto character = GetOwner<Character>();
	if(!character) return;
	auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
	auto movementComp = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	if(!stateComponent) return;

	character->SetActive(false);
	stateComponent->SetState(State::DEAD);
	LOG_DEBUG("HealthComponent::OnDeath");
	if(movementComp) movementComp->SetDesiredDirection(glm::vec2(0.0f, 0.0f));		// 移動向量設爲0
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)->UnregisterNGameObject(character);

	auto trackingManager = scene->GetManager<TrackingManager>(ManagerTypes::TRACKING);
	if(character->GetType() == CharacterType::ENEMY) {
		trackingManager->RemoveEnemy(character);
		LOG_DEBUG("HealthComponent::remove ");
	}
	else
	{
		trackingManager->SetPlayer(nullptr);
	}
}