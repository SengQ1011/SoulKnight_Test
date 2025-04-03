//
// Created by tjx20 on 3/26/2025.
//

#ifndef ATTACKCOMPONENT_HPP
#define ATTACKCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Weapon/Weapon.hpp"

class AttackComponent: public Component {
public:
	explicit AttackComponent(std::shared_ptr<Weapon> initWeapon, float criticalRate, int handBladeDamage, int collisionDamage);
	~AttackComponent() override = default;

	void Init() override;
	void Update() override;

	//----Getter----
	[[nodiscard]] float GetAimRange() const { return m_aimRange;}
	[[nodiscard]] int GetMaxWeapon() const { return m_maxWeapon; }
	[[nodiscard]] std::shared_ptr<Weapon> GetCurrentWeapon() const { return m_currentWeapon; }
	[[nodiscard]] std::vector<std::shared_ptr<Weapon>> GetAllWeapons() const { return m_Weapons; }


	//----Setter----
	void SetMaxWeapon(int num) { m_maxWeapon = num; }
	void AddWeapon(std::shared_ptr<Weapon> weapon);
	void RemoveWeapon();
	void switchWeapon();

	int calculateDamage();
	void TryAttack();		// 使用當前武器攻擊敵人
	void SetDualWield(const bool enable); // 雙武器

private:
	float m_aimRange;			// 自動瞄準範圍
	float m_criticalRate;		// 攻擊暴擊率(計算傷害公式=(角色+武器)暴擊率 * 武器傷害)
	int m_maxWeapon;			// 最多武器數量
	int m_handBladeDamage;		// player:手刀傷害
	int m_collisionDamage;		// enemy:專用於Collision模式的傷害值
	std::shared_ptr<Weapon> m_currentWeapon;				// 目前裝備的武器
	std::vector<std::shared_ptr<Weapon>> m_Weapons;		// 每個角色都有武器
};

#endif //ATTACKCOMPONENT_HPP
