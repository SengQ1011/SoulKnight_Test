//
// Created by tjx20 on 3/24/2025.
//

#ifndef HEALTHCOMPONENT_HPP
#define HEALTHCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Structs/CollisionComponentStruct.hpp"


class HealthComponent : public Component
{
public:
	explicit HealthComponent(const int maxHp, const int maxArmor, const int maxEnergy);
	~HealthComponent() override = default;

	void Update() override;

	//----Getter----
	int GetMaxHp() const { return m_maxHp; }
	int GetCurrentHp() const { return m_currentHp; }
	int GetMaxArmor() const { return m_maxArmor; }
	int GetCurrentArmor() const { return m_currentArmor; }
	int GetMaxEnergy() const { return m_maxEnergy; }
	int GetCurrentEnergy() const { return m_currentEnergy; }

	//----Setter----
	void SetMaxHp(const int hp) { m_maxHp = hp; }
	void AddCurrentHp(const int hp) { m_currentHp += hp; }
	void SetMaxArmor(const int armor) { m_maxArmor = armor; }
	void AddCurrentArmor(const int armor) { m_currentArmor += armor; }
	void SetMaxEnergy(const int energy) { m_maxEnergy = energy; }
	void AddCurrentEnergy(const int energy) { m_currentEnergy += energy; }
	void ConsumeEnergy(const int energy) { m_currentEnergy -= energy; }
	void SetBreakProtection(const bool breakProtection) { m_breakProtection = breakProtection; }

	void TakeDamage(int damage);
	void HandleCollision(const CollisionEventInfo &info) override;
	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

private:
	int m_maxHp; // 生命上限
	int m_currentHp; // 當前生命值
	int m_maxArmor; // 護甲上限
	int m_currentArmor; // 當前護甲值
	int m_maxEnergy; // 能量上限
	int m_currentEnergy; // 當前能量值
	float m_armorRecoveryInterval = 1.5f;
	float m_armorRecoveryTimer = 0.0f; // 恢復護甲計時器
	bool m_breakProtection = false; // 天賦：破甲保護
	std::unordered_map<nGameObject*, float> m_recentAttackSources;	// 用rawPointer記錄(因爲利用，可能會連續使用同樣的子彈)
	float m_invincibleDuration = 0.5f; // 碰撞後對於同一個物件無敵時間（秒）

	// 通知 StateComponent 角色死亡
	void OnDeath() const;
};



#endif //HEALTHCOMPONENT_HPP
