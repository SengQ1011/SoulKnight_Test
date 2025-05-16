//
// Created by QuzzS on 2025/3/4.
//

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Override/nGameObject.hpp"

class Character;

class Weapon: public nGameObject {
public:
	explicit Weapon(const std::string& ImagePath, const std::string& name, int damage, int energy, float criticalRate, int offset, float attackInterval);
	~Weapon() override = default;

	//----Getter----
	std::string GetImagePath() const { return m_ImagePath; }
	std::string GetName() const override { return m_weaponName; }
	int GetDamage() const{ return m_damage; }
	int GetEnergy() const { return m_energy; }
	float GetCriticalRate() const { return m_criticalRate; }
	float GetAttackInterval() const { return m_attackInterval; }
	int GetOffset() const { return m_offset; }
	std::shared_ptr<Character> GetWeaponOwner() const { return m_currentOwner;}
	[[nodiscard]] bool GetIsSword() const { return m_IsSword; }

	//----Setter----
	void SetImage(const std::string& ImagePath);
	void SetLastAttackTime(const float time) { lastAttackTime = time; }
	void SetOwner(std::shared_ptr<Character> owner) { m_currentOwner = owner; }
	void RemoveOwner() { m_currentOwner = nullptr; }
	void SetIsSword(const bool is) { m_IsSword = is; }
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
	std::string m_ImagePath;		// 武器照片
	std::string m_weaponName;		// 武器名稱
	int m_damage;					// 武器傷害
	int m_energy;					// 武器所需能量
	float m_criticalRate;			// 武器暴擊率
	float m_attackInterval;			// 攻擊頻率
	int m_offset;					// 攻擊偏移量
	float m_attackDelay = 0.0f;		// 開始攻擊延遲
	bool m_IsSword = false;
	std::shared_ptr<Character> m_currentOwner;

	float lastAttackTime = 0.0f;  // 上次攻擊的時間
};
#endif //WEAPON_HPP
