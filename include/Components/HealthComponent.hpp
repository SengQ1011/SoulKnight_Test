//
// Created by tjx20 on 3/24/2025.
//

#ifndef HEALTHCOMPONENT_HPP
#define HEALTHCOMPONENT_HPP

#include "Components/Component.hpp"

class HealthComponent : public Component {
public:
	explicit HealthComponent(const int maxHp, const  int maxArmor, const  int maxEnergy);
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
	void SetMaxHp(int hp) { m_maxHp = hp; }
	void AddCurrentHp(int hp) { m_currentHp += hp; }
	void SetMaxArmor(int armor) { m_maxArmor = armor; }
	void AddCurrentArmor(int armor) {m_currentArmor += armor; }
	void SetMaxEnergy(int energy) { m_maxEnergy = energy; }
	void AddCurrentEnergy(int energy) { m_currentEnergy += energy; }
	void ConsumeEnergy(int energy) {m_currentEnergy -= energy; }

	void TakeDamage(int damage);

private:
		int m_maxHp;        // 生命上限
		int m_currentHp;    // 當前生命值
	int m_maxArmor;         // 護甲上限
	int m_currentArmor;     // 當前護甲值
	int m_maxEnergy;        // 能量上限
	int m_currentEnergy;    // 當前能量值

	float m_armorRecoveryInterval = 3.0f;
	float m_armorRecoveryTimer; // 恢復護甲計時器

	// 通知 StateComponent 角色死亡
	void OnDeath();
};


#endif //HEALTHCOMPONENT_HPP
