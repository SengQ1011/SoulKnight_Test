//
// Created by tjx20 on 3/26/2025.
//

#ifndef GUN_HPP
#define GUN_HPP

#include "Attack/EffectAttack.hpp"
#include "Attack/Projectile.hpp"
#include "Weapon/Weapon.hpp"

struct GunWeaponInfo : public BaseWeaponInfo
{
	int numOfBullets = 1;
	bool bulletCanTracking = false;

	ProjectileInfo defaultProjectileInfo;
};

class GunWeapon final : public Weapon
{
public:
	explicit GunWeapon(const GunWeaponInfo &gunWeaponInfo);
	~GunWeapon() override = default;

	void attack(int damage, bool isCriticalHit = false) override;
	std::shared_ptr<Weapon> Clone() const override { return std::make_shared<GunWeapon>(*this); }


private:
	int m_numOfBullets;
	bool m_bulletCanTracking;

	const ProjectileInfo m_projectileInfo;
};

#endif // GUN_HPP
