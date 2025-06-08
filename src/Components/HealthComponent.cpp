//
// Created by tjx20 on 3/28/2025.
//

#include "Components/HealthComponent.hpp"

#include "Components/AttackComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"
#include "ObserveManager/EventManager.hpp"
#include "ObserveManager/TrackingManager.hpp"
#include "Room/RoomCollisionManager.hpp"


#include "Scene/SceneManager.hpp"
#include "Util/Time.hpp"

#include "Attack/Attack.hpp"
#include "Creature/Character.hpp"
#include "Structs/DeathEventInfo.hpp"
#include "Structs/TakeDamageEventInfo.hpp"


class StateComponent;
HealthComponent::HealthComponent(const int maxHp, const int maxArmor = 0, const int maxEnergy = 0) :
	Component(ComponentType::HEALTH), m_maxHp(maxHp), m_currentHp(maxHp), m_maxArmor(maxArmor),
	m_currentArmor(maxArmor), m_maxEnergy(maxEnergy), m_currentEnergy(maxEnergy)
{
}

void HealthComponent::Update()
{
	const float deltaTime = Util::Time::GetDeltaTimeMs() / 1000.0f;
	// 減少所有來源的冷卻時間
	for (auto it = m_recentAttackSources.begin(); it != m_recentAttackSources.end();)
	{
		it->second -= deltaTime;
		if (it->second <= 0)
		{
			it = m_recentAttackSources.erase(it);
		}
		else
			++it;
	}

	if (m_maxArmor == 0)
		return;
	if (m_currentArmor < m_maxArmor)
	{
		m_armorRecoveryTimer += deltaTime;
		if (m_armorRecoveryTimer >= m_armorRecoveryInterval)
		{
			AddCurrentArmor(1);
			m_armorRecoveryTimer = 0.0f;
		}
	}
	if (m_currentHp <= 0) OnDeath();
}

void HealthComponent::HandleEvent(const EventInfo &eventInfo)
{
	// {}可以在case裏形成額外作用域，用來在裏面定義變數
	switch (eventInfo.GetEventType())
	{
	case EventType::Collision: // 僅處理碰撞傷害
		{
			const auto &collisionEventInfo = dynamic_cast<const CollisionEventInfo &>(eventInfo);
			HandleCollision(collisionEventInfo);
			break;
		}
	case EventType::TakeDamage:
		{
			const auto &dmgInfo = dynamic_cast<const TakeDamageEventInfo &>(eventInfo);
			const auto ObjectID = dmgInfo.m_Id;
			// 冷卻中就不處理
			if (m_recentAttackSources.count(ObjectID) > 0)
				break;
			m_recentAttackSources[ObjectID] = m_invincibleDuration;
			TakeDamage(dmgInfo.damage);

			const auto owner = GetOwner<nGameObject>();
			if (!owner)
				return;

			// 元素傷害
			if (dmgInfo.elementalDamage != StatusEffect::NONE)
			{
				const auto owner = GetOwner<nGameObject>();
				const auto stateComp = owner->GetComponent<StateComponent>(ComponentType::STATE);
				if (!stateComp)
					return;
				stateComp->ApplyStatusEffect(dmgInfo.elementalDamage);
			}

			// 角色特效
			const auto character = std::dynamic_pointer_cast<Character>(owner);
			if (!character || character->GetType() != CharacterType::PLAYER)
				return;

			// 觸發Camera抖動
			EventManager::TriggerCameraShake();
			break;
		}
	default:
		break;
	}
}

std::vector<EventType> HealthComponent::SubscribedEventTypes() const
{
	return {
		EventType::Collision,
		EventType::TakeDamage,
	};
}

// 只處理碰撞傷害 - 被怪物撞、陷阱、尖刺
void HealthComponent::HandleCollision(const CollisionEventInfo &info)
{
	auto collisionObject = info.GetObjectB();
	if (!collisionObject)
		return;

	const auto ObjectID = collisionObject->GetID();

	// 冷卻中就不處理
	if (m_recentAttackSources.count(ObjectID) > 0)
		return;
	m_recentAttackSources[ObjectID] = m_invincibleDuration;

	// collisionEnemy的碰撞傷害
	if (const auto character = std::dynamic_pointer_cast<Character>(info.GetObjectB()))
	{
		if (character->GetType() == CharacterType::ENEMY)
		{
			if (const auto collisionDamage =
					character->GetComponent<AttackComponent>(ComponentType::ATTACK)->GetCollisionDamage();
				collisionDamage > 0)
			{
				// LOG_DEBUG("Enemy collision damage = {}", collisionDamage);

				this->TakeDamage(collisionDamage);
			}
		}
	}

	// TODO-尖刺
}

void HealthComponent::TakeDamage(int damage)
{
	// 無敵模式
	if (m_invincibleMode)
		return;
	LOG_DEBUG("take damage {}", damage);

	if (const auto owner = GetOwner<nGameObject>())
	{
		// 觸發閃爍特效 - 使用OnEvent進行組件間通信
		StartFlickerEvent flickerEvent(0.5f, 0.05f);
		owner->OnEvent(flickerEvent);
	}

	// 天賦：破甲保護
	if (m_breakProtection && damage > m_currentArmor && m_currentArmor > 0)
	{
		m_currentArmor = 0; // 只扣盔甲
		return;
	}
	int remainingDamage = std::max(0, damage - m_currentArmor);
	m_currentArmor = std::max(0, m_currentArmor - damage);
	m_currentHp = std::max(0, m_currentHp - remainingDamage);

	if (m_currentHp <= 0)
	{
		OnDeath();
	}
}

void HealthComponent::OnDeath() const
{
	auto character = GetOwner<Character>();
	if (!character)
		return;
	auto stateComponent = character->GetComponent<StateComponent>(ComponentType::STATE);
	auto movementComp = character->GetComponent<MovementComponent>(ComponentType::MOVEMENT);
	// TODO:銷毀武器
	// if (const auto attackComp = character->GetComponent<AttackComponent>(ComponentType::ATTACK)) {
	// 	attackComp->RemoveAllWeapon();
	// }


	const DeathEventInfo deathEventInfo{character};
	character->OnEvent(deathEventInfo);

	stateComponent->SetState(State::DEAD);
	stateComponent->HideAllIcon();
	character->SetActive(false);

	if (movementComp)
		movementComp->SetDesiredDirection(glm::vec2(0.0f, 0.0f)); // 移動向量設爲0
	auto scene = SceneManager::GetInstance().GetCurrentScene().lock();
	scene->GetCurrentRoom()
		->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION)
		->UnregisterNGameObject(character);

	auto trackingManager = scene->GetCurrentRoom()->GetManager<TrackingManager>(ManagerTypes::TRACKING);
	if (character->GetType() == CharacterType::ENEMY)
	{
		trackingManager->RemoveEnemy(character);

		// 使用事件系統通知敵人死亡
		EnemyDeathEvent enemyDeathEvent(character);
		EventManager::GetInstance().Emit(enemyDeathEvent);
		// 通知場景纍加killCount
		EventManager::enemyDeathEvent();

		LOG_DEBUG("HealthComponent::Enemy died, event sent");
		if (auto aiComp = character->GetComponent<AIComponent>(ComponentType::AI))
		{
			aiComp->HideReadyAttackIcon();
		}
	}
	else
	{
		trackingManager->SetPlayer(nullptr);
		// EventManager::GetInstance().Notify(deathEventInfo);
	}
}
