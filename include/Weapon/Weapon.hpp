//
// Created by QuzzS on 2025/3/4.
//

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Override/nGameObject.hpp"
#include "Creature/Character.hpp"
#include "BulletManager.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

class Weapon: public nGameObject {
public:
	explicit Weapon(const std::string& ImagePath, const std::string& bulletImagePath,const std::string& name, int damage, int energy, float criticalRate, int offset, float attackSpeed);
	~Weapon() override = default;

	//----Getter----
	std::string GetImagePath() const { return m_ImagePath; }
	std::string GetName() const { return m_weaponName; }
	int GetDamage() const{ return m_damage; }
	int GetEnergy() const { return m_energy; }
	float GetCriticalRate() const { return m_criticalRate; }
	float GetAttackSpeed() const { return m_attackColdDown; }
	int GetOffset() const { return m_offset; }

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
		lastAttackTime = m_attackColdDown + m_attackDelay; // 冷却时间 + 初始延迟
	}

	void UpdateCooldown(float deltaTime);
	int calculateDamage();
	bool CanAttack();
	virtual void attack(int damage) = 0;
	// 支持克隆複製
	virtual std::shared_ptr<Weapon> Clone() const = 0;


protected:
	std::string m_ImagePath;		// 武器照片
	std::string m_bulletImagePath;	// 子彈照片
	std::string m_weaponName;		// 武器名稱
	int m_damage;					// 武器傷害
	int m_energy;					// 武器所需能量
	float m_criticalRate;			// 武器暴擊率
	float m_attackColdDown;			// 攻擊頻率
	int m_offset;					// 攻擊偏移量
	float m_attackDelay = 0.0f;		// 開始攻擊延遲
	std::shared_ptr<Character> m_currentOwner;

	float lastAttackTime = 0.0f;  // 上次攻擊的時間
};
#endif //WEAPON_HPP
