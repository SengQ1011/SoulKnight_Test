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
						 const std::unordered_map<AttackType, std::shared_ptr<IAttackStrategy>> &attackStrategies,
						 const std::shared_ptr<IUtilityStrategy> &utilityStrategies, int monsterPoint);
	~AIComponent() override = default;

	void Init() override;
	void Update() override;

	//----Getter----
	[[nodiscard]] MonsterType GetMonsterType() const { return m_aiType; }
	[[nodiscard]] int GetMonsterPoint() const { return m_monsterPoint; }
	[[nodiscard]] enemyState GetEnemyState() const { return m_enemyState; }
	[[nodiscard]] std::weak_ptr<nGameObject> GetTarget() const { return m_Target; }

	//----Setter----
	void RemoveTarget() { m_Target.reset(); }
	void OnPlayerPositionUpdate(std::weak_ptr<Character> player) override;
	void OnLostPlayer() override { m_Target.reset(); }
	void SetEnemyState(enemyState state) const;

	void HandleCollision(CollisionInfo &info) override;

protected:
	MonsterType m_aiType;
	enemyState m_enemyState;
	int m_monsterPoint;
	std::weak_ptr<nGameObject> m_Target; // enemy鎖定目標的位置
	float m_behaviorTimer = 0;
	float m_attackCooldown = 0;

	EnemyContext m_context;
	std::shared_ptr<IMoveStrategy> m_moveStrategy;
	std::unordered_map<AttackType, std::shared_ptr<IAttackStrategy>> m_attackStrategy;
	std::shared_ptr<IUtilityStrategy> m_utilityStrategy;
};


#endif //AICOMPONENT_HPP