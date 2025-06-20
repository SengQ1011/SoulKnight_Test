//
// Created by tjx20 on 3/24/2025.
//

#ifndef AICOMPONENT_HPP
#define AICOMPONENT_HPP

#include "Component.hpp"
#include "Observer.hpp"

#include "StructType.hpp"
#include "Structs/CollisionComponentStruct.hpp"


class IMoveStrategy;
class IAttackStrategy;
class IUtilityStrategy;

class AIComponent : public Component, public TrackingObserver
{
public:
	explicit AIComponent(MonsterType MonsterType, const std::shared_ptr<IMoveStrategy> &moveStrategy,
						 const std::unordered_map<AttackStrategies, std::shared_ptr<IAttackStrategy>> &attackStrategies,
						 const std::shared_ptr<IUtilityStrategy> &utilityStrategies, int monsterPoint);
	~AIComponent() override = default;

	void Init() override;
	void Update() override;

	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

	//----Getter----
	[[nodiscard]] MonsterType GetMonsterType() const { return m_aiType; }
	[[nodiscard]] int GetMonsterPoint() const { return m_monsterPoint; }
	[[nodiscard]] enemyState GetEnemyState() const { return m_enemyState; }
	[[nodiscard]] std::weak_ptr<nGameObject> GetTarget() const { return m_Target; }
	[[nodiscard]] float GetReadyAttackCountdown() const { return m_readyAttackTime; }
	[[nodiscard]] float GetReadyAttackTimer() const { return m_readyAttackTimer; }
	[[nodiscard]] float GetSkillTimer() const { return m_skillTimer; }
	[[nodiscard]] std::shared_ptr<IAttackStrategy> GetAttackStrategy(AttackStrategies type) const {
		auto it = m_attackStrategy.find(type);
		if (it != m_attackStrategy.end()) {
			return it->second;
		}
		return nullptr;
	}

	//----Setter----
	void RemoveTarget() { m_Target.reset(); }
	void OnPlayerPositionUpdate(std::weak_ptr<Character> player) override;
	void OnLostPlayer() override;
	void SetEnemyState(enemyState state);
	void ResetReadyAttackTimer() { m_readyAttackTimer = m_readyAttackTime; }
	void DeductionReadyAttackTimer(const float time) { m_readyAttackTimer -= time; }
	void SetSkillTimer(float duration) { m_skillTimer = duration; }
	void DeductionSkillTimer(float deltaTime) { m_skillTimer -= deltaTime; }

	void ShowReadyAttackIcon() const;
	void HideReadyAttackIcon();


protected:
	MonsterType m_aiType;
	enemyState m_enemyState;
	EnemyContext m_context;
	int m_monsterPoint;
	std::shared_ptr<AttackComponent> m_AttackComponent;	// 缓存 AttackComponent
	std::weak_ptr<nGameObject> m_Target; // enemy鎖定目標的位置
	float m_attackRange = 0;
	const float m_readyAttackTime = 1.5f;
	float m_readyAttackTimer = 0.0f;
	float m_skillTimer = 0.0f;  // 技能計時器
	glm::vec2 m_iconOffset = glm::vec2(-15.0f, 15.0f);
	std::shared_ptr<nGameObject> m_readyAttackIcon;
	std::shared_ptr<IMoveStrategy> m_moveStrategy;
	std::unordered_map<AttackStrategies, std::shared_ptr<IAttackStrategy>> m_attackStrategy;
	std::shared_ptr<IUtilityStrategy> m_utilityStrategy;
};


#endif //AICOMPONENT_HPP