//
// Created by tjx20 on 3/26/2025.
//

//AttackComponent.hpp

#ifndef ATTACKCOMPONENT_HPP
#define ATTACKCOMPONENT_HPP

#include "Components/Component.hpp"
#include "Weapon/Weapon.hpp"

class AttackComponent final : public Component {
public:
	explicit AttackComponent(const std::shared_ptr<Weapon> &initWeapon, const float criticalRate, const int handBladeDamage, const int collisionDamage);
	~AttackComponent() override = default;

	void Init() override;
	void Update() override;

	//----Getter----
	[[nodiscard]] float GetAimRange() const { return m_aimRange;}
	[[nodiscard]] int GetMaxWeapon() const { return m_maxWeapon; }
	[[nodiscard]] std::shared_ptr<Weapon> GetCurrentWeapon() const { return m_currentWeapon; }
	[[nodiscard]] std::shared_ptr<Weapon> GetSecondWeapon() const { return m_secondWeapon; }
	[[nodiscard]] std::vector<std::shared_ptr<Weapon>> GetAllWeapons() const { return m_Weapons; }
	[[nodiscard]] int GetNumRebound() const { return m_numRebound; }
	[[nodiscard]] bool GetReflectBullet() const { return m_ReflectBullets; }
	[[nodiscard]] int GetCollisionDamage() const { return m_collisionDamage; }

	//----Setter----
	void SetMaxWeapon(const int num) { m_maxWeapon = num; }
	void AddWeapon(std::shared_ptr<Weapon> weapon);
	void RemoveWeapon();
	void switchWeapon();
	void SetNumRebound(const int num) { m_numRebound = num; }
	void SetReflectBullet(const bool enable) { m_ReflectBullets = enable; }
	void SetDualWield(const bool enable); // 雙武器
	void SetSecondWeapon(const std::shared_ptr<Weapon> &weapon)
	{
		LOG_DEBUG("Check set weapon");
		if(!weapon) LOG_ERROR("weapon=nullptr");
		m_secondWeapon = weapon;
	}

	int calculateDamage();
	void TryAttack();		// 使用當前武器攻擊敵人

private:
	float m_aimRange;				// 自動瞄準範圍
	float m_criticalRate;			// 攻擊暴擊率(計算傷害公式=(角色+武器)暴擊率 * 武器傷害)
	int m_maxWeapon;				// 最多武器數量
	int m_handBladeDamage;			// player:手刀傷害
	int m_collisionDamage;			// enemy:專用於Collision模式的傷害值
	int m_numRebound = 0;			// 天賦：子彈反彈
	bool m_ReflectBullets = false;	// 天賦：近戰反彈
	bool m_dualWield = false;		// 是否雙持武器
	std::shared_ptr<Weapon> m_currentWeapon;			// 目前裝備的武器
	std::shared_ptr<Weapon> m_secondWeapon;				// 技能：雙持武器
	std::vector<std::shared_ptr<Weapon>> m_Weapons;		// 每個角色都有武器
};

#endif //ATTACKCOMPONENT_HPP
