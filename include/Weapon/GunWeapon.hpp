//
// Created by tjx20 on 3/26/2025.
//

#ifndef GUN_HPP
#define GUN_HPP

#include "Weapon/Weapon.hpp"

struct GunWeaponInfo : public BaseWeaponInfo {
	std::string bulletImagePath;
	float bulletSize = 1.0f;
	float bulletSpeed = 1.0f;
	bool bulletCanReboundBySword = false;
	bool bulletIsBubble = false;
	bool haveBubbleTrail = false;
	std::string bubbleImagePath = std::string(RESOURCE_DIR) + "/attackUI/bullet/bullet_107.png";
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
	float m_bulletSize;
	float m_bulletSpeed;
	bool m_bulletCanReboundBySword;
	bool m_bulletIsBubble;
	bool m_bulletHaveBubbleTrail;
	std::string m_bubbleImagePath;
};

#endif //GUN_HPP
