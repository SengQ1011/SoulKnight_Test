//
// Created by tjx20 on 3/26/2025.
//

#ifndef GUN_HPP
#define GUN_HPP

#include "Weapon/Weapon.hpp"

struct GunWeaponInfo : public BaseWeaponInfo {
	std::string bulletImagePath;
	int numOfBullets = 1;
	float bulletOffset = 0;
	float bulletSize = 1.0f;
	float bulletSpeed = 1.0f;
	bool bulletCanReboundBySword = false;
	bool bulletIsBubble = false;
	bool haveBubbleTrail = false;
	std::string bubbleImagePath = std::string(RESOURCE_DIR) + "/attackUI/bullet/bullet_107.png";
	bool haveEffectAttack = false;
	float effectAttackSize = 0.0f;
	int effectAttackDamage = 0;
	EffectAttackType effect = EffectAttackType::NONE;
};

class GunWeapon final : public Weapon {
public:
	explicit GunWeapon(const GunWeaponInfo& gunWeaponInfo);
	~GunWeapon() override = default;

	void attack(int damage) override;
	std::shared_ptr<Weapon> Clone() const override {
		return std::make_shared<GunWeapon>(*this);
	}


private:
	std::string m_bulletImagePath;	// 子彈照片
	int m_numOfBullets;
	float m_bulletOffset;
	float m_bulletSize;
	float m_bulletSpeed;
	bool m_bulletCanReboundBySword;
	bool m_bulletIsBubble;
	bool m_bulletHaveBubbleTrail;
	std::string m_bubbleImagePath;
	bool m_bulletHaveEffectAttack = false;
	float m_effectAttackSize = 0.0f;
	int m_effectAttackDamage = 0;
	EffectAttackType m_bullet_EffectAttack;
};

#endif //GUN_HPP
