//
// Created by QuzzS on 2025/3/4.
//

#ifndef WEAPON_HPP
#define WEAPON_HPP

#include "Override/nGameObject.hpp"
#include "Util/Image.hpp"

class Weapon: public nGameObject{
public:
	explicit Weapon(const std::string& ImagePath, const std::string name, int damage, int energy, float criticalRate, int offset, int dropLevel,  float attackRange, float size, int attackSpeed);
	~Weapon() override = default;

	std::string getImagePath() const { return m_ImagePath; }
	void SetImage(const std::string& ImagePath);
	std::string getName() const { return m_weaponName; }
	int getDamage() const{ return m_damage; }
	void attack();

private:
	std::string m_ImagePath;
	std::string m_weaponName;
	int m_damage;			// 傷害
	int m_energy;
	float m_criticalRate;
	int m_offset;
	int m_dropLevel;
	float m_attackRange;	// 攻擊範圍（刀劍）
	float m_bulletSize;	// 子彈大小（槍支）
	int m_attackSpeed;	// 攻速
};

#endif //WEAPON_HPP
