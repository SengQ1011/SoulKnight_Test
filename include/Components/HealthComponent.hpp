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
	[[nodiscard]] int GetMaxHp() const { return m_maxHp; }
	[[nodiscard]] int GetCurrentHp() const { return m_currentHp; }
	[[nodiscard]] int GetMaxArmor() const { return m_maxArmor; }
	[[nodiscard]] int GetCurrentArmor() const { return m_currentArmor; }
	[[nodiscard]] int GetMaxEnergy() const { return m_maxEnergy; }
	[[nodiscard]] int GetCurrentEnergy() const { return m_currentEnergy; }
	[[nodiscard]] bool GetIsInvincibleMode() const { return m_invincibleMode; }

	//----Setter----
	void SetMaxHp(const int hp) { m_maxHp = hp; }
	void SetCurrentHp(const int hp) { m_currentHp = hp; }
	void AddCurrentHp(const int hp){
		if (m_currentHp + hp <= m_maxHp) m_currentHp += hp;
		else m_currentHp = m_maxHp;
	}
	void SetMaxArmor(const int armor) { m_maxArmor = armor; }
	void AddCurrentArmor(const int armor)
	{
		if (m_currentArmor + armor <= m_maxArmor) m_currentArmor += armor;
		else m_currentArmor = m_maxArmor;
	}
	void SetMaxEnergy(const int energy) { m_maxEnergy = energy; }
	void SetCurrentEnergy(const int energy) { m_currentEnergy = energy; }
	void AddCurrentEnergy(const int energy)
	{
		if (m_currentEnergy + energy <= m_maxEnergy ) m_currentEnergy += energy;
		else m_currentEnergy = m_maxEnergy;
	}
	void ConsumeEnergy(const int energy) { if (m_currentEnergy - energy > 0) m_currentEnergy -= energy; }
	void SetInvincibleMode(const bool op) { m_invincibleMode = op; }
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
	float m_armorRecoveryInterval = 2.0f;
	float m_armorRecoveryTimer = 0.0f;	// 恢復護甲計時器
	bool m_invincibleMode = false;		// 無敵模式
	bool m_breakProtection = false;		// 天賦：破甲保護
	std::unordered_map<int, float> m_recentAttackSources;	// 用rawPointer記錄(因爲利用，可能會連續使用同樣的子彈)
	float m_invincibleDuration = 0.5f; // 碰撞後對於同一個物件無敵時間（秒）

	// 通知 StateComponent 角色死亡
	void OnDeath() const;
};



#endif //HEALTHCOMPONENT_HPP
