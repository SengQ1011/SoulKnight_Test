//
// Created by tjx20 on 3/26/2025.
//

#ifndef MELEEWEAPON_HPP
#define MELEEWEAPON_HPP

#include "Weapon.hpp"

class MeleeWeapon : public Weapon {
public:
	explicit MeleeWeapon(const std::string& ImagePath, const std::string& name, int damage,
		int energy, float criticalRate, int offset, float attackInterval, float attackRange,
		bool isSword, const EffectAttackType type);
	~MeleeWeapon() override = default;

	void attack(int damage) override;
	std::shared_ptr<Weapon> Clone() const override {
		return std::make_shared<MeleeWeapon>(*this);
	}

	//----Getter----
	[[nodiscard]] EffectAttackType GetEffectAttackType() const { return m_effectAttackType; }

private:
	EffectAttackType m_effectAttackType;
	float m_attackRange;  // 近戰武器的攻擊範圍
};

#endif // MELEEWEAPON_HPP