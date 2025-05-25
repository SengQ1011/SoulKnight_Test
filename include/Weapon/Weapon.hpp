//
// Created by QuzzS on 2025/3/4.
//

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Override/nGameObject.hpp"

class Character;

struct BaseWeaponInfo {
	std::string imagePath;
	std::string name;
	AttackType attackType;
	WeaponType weaponType;
	int damage = 0;
	int energy = 0;
	float criticalRate = 0.0f;
	int offset = 0;
	float attackInterval = 0.0f;
	int dropLevel = 0;
	int basicPrice = 0;
};

class Weapon: public nGameObject {
public:
	explicit Weapon(const BaseWeaponInfo& weaponInfo);
	~Weapon() override = default;

	//----Getter----
	[[nodiscard]] AttackType GetAttackType() const {return m_AttackType; }
	[[nodiscard]] WeaponType GetWeaponType() const {return m_weaponType; }
	[[nodiscard]] std::string GetImagePath() const { return m_ImagePath; }
	[[nodiscard]] std::string GetName() const override { return m_weaponName; }
	[[nodiscard]] int GetDamage() const{ return m_damage; }
	[[nodiscard]] int GetEnergy() const { return m_energy; }
	[[nodiscard]] float GetCriticalRate() const { return m_criticalRate; }
	[[nodiscard]] float GetAttackInterval() const { return m_attackInterval; }
	[[nodiscard]] int GetOffset() const { return m_offset; }
	[[nodiscard]] std::shared_ptr<Character> GetWeaponOwner() const { return m_currentOwner;}
	[[nodiscard]] static bool weaponHasOffset(const AttackType attackType, const WeaponType weaponType)
				{ return attackType == AttackType::EFFECT_ATTACK && weaponType != WeaponType::SPEAR; }

	//----Setter----
	void SetImage(const std::string& ImagePath);
	void SetLastAttackTime(const float time) { lastAttackTime = time; }
	void SetOwner(std::shared_ptr<Character> owner) { m_currentOwner = owner; }
	void RemoveOwner() { m_currentOwner = nullptr; }
	void SetAttackDelay(float delay) {
		m_attackDelay = delay;
		lastAttackTime = m_attackDelay;
	}
	void ResetAttackTimer() {
		lastAttackTime = m_attackInterval + m_attackDelay; // 冷却时间 + 初始延迟
	}

	void UpdateCooldown(float deltaTime);
	int calculateDamage();
	bool CanAttack();
	virtual void attack(int damage) = 0;
	// 支持克隆複製
	virtual std::shared_ptr<Weapon> Clone() const = 0;


protected:
	AttackType m_AttackType = AttackType::NONE;
	WeaponType m_weaponType = WeaponType::NONE;
	std::string m_ImagePath;		// 武器照片
	std::string m_weaponName;		// 武器名稱
	int m_damage;					// 武器傷害
	int m_energy;					// 武器所需能量
	float m_criticalRate;			// 武器暴擊率
	float m_attackInterval;			// 攻擊頻率
	int m_offset;					// 攻擊偏移量
	int m_dropLevel = 0;
	int m_basicPrice = 0;

	float m_attackDelay = 0.0f;		// 開始攻擊延遲
	float lastAttackTime = 0.0f;  // 上次攻擊的時間

	std::shared_ptr<Character> m_currentOwner;

};
#endif //WEAPON_HPP
