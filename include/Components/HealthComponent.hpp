//
// Created by tjx20 on 3/24/2025.
//

#ifndef HEALTHCOMPONENT_HPP
#define HEALTHCOMPONENT_HPP

#include "Components/Component.hpp"

class HealthComponent : public Component {
public:
	explicit HealthComponent(float maxHP, float armor);
	~HealthComponent() override = default;

	void TakeDamage(float damage) {
		float finalDamage = damage * (1.0f - armor); // 根據護甲減傷
		currentHP -= finalDamage;

		if (currentHP <= 0) {
			currentHP = 0;
			OnDeath();
		}
	}

	void Heal(float amount) {
		currentHP += amount;
		if (currentHP > maxHP) {
			currentHP = maxHP;
		}
	}

	float GetHP() const { return currentHP; }


private:
	float maxHP;
	float currentHP;
	float armor;

	// 通知 StateComponent 角色死亡
	void OnDeath();
};


#endif //HEALTHCOMPONENT_HPP
