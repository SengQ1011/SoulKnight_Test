//
// Created by tjx20 on 3/26/2025.
//

#ifndef MELEEWEAPON_HPP
#define MELEEWEAPON_HPP

#include "Attack/EffectAttack.hpp"
#include "Weapon.hpp"

struct MeleeWeaponInfo : public BaseWeaponInfo {
	float attackRange = 0.0f;
	EffectAttackInfo defaultEffectAttackInfo;
};

class MeleeWeapon : public Weapon {
public:
	explicit MeleeWeapon(const MeleeWeaponInfo& meleeWeaponInfo);
	~MeleeWeapon() override = default;

	void attack(int damage) override;
	std::shared_ptr<Weapon> Clone() const override {
		return std::make_shared<MeleeWeapon>(*this);
	}

	//----Getter----
	[[nodiscard]] float GetAttackRange() const { return m_attackRange; }

private:
	float m_attackRange;  // 近戰武器的攻擊範圍
	EffectAttackInfo m_effectAttackInfo;
};

#endif // MELEEWEAPON_HPP