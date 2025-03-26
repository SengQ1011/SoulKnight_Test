//
// Created by tjx20 on 3/26/2025.
//

#ifndef MELEE_HPP
#define MELEE_HPP

#include "Weapon/Weapon.hpp"

#ifndef MELEEWEAPON_HPP
#define MELEEWEAPON_HPP

#include "Weapon.hpp"
#include <iostream>

class MeleeWeapon : public Weapon {
public:
	explicit MeleeWeapon(const std::string& ImagePath, const std::string& bulletImagePath, const std::string& name, int damage, int energy, float criticalRate, int offset, float attackSpeed, float attackRange);
	~MeleeWeapon() override = default;

	void attack(int damage) override;

private:
	float m_attackRange;  // 近戰武器的攻擊範圍
};

#endif // MELEEWEAPON_HPP


#endif //MELEE_HPP
