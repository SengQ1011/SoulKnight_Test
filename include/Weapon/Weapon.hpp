//
// Created by QuzzS on 2025/3/4.
//

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Override/nGameObject.hpp"

class Weapon: public nGameObject{
public:
	int damage;			// 傷害
	int attackRange;	// 攻擊範圍（刀劍）
	int size;			// 子彈大小（槍支）
	int attackSpeed;	// 攻速

	explicit Weapon(const std::string& ImagePath, int damage);
	~Weapon() override = default;
};

#endif //WEAPON_HPP
