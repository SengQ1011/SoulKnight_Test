//
// Created by tjx20 on 3/10/2025.
//
// WeaponFactory.cpp

#include "Factory/WeaponFactory.hpp"

#include "Attack/EffectAttack.hpp"
#include "Factory/Factory.hpp"

#include "Weapon/GunWeapon.hpp"
#include "Weapon/MeleeWeapon.hpp"

static const std::unordered_map<std::string, WeaponType> weaponTypeMap = {
	{"NONE", WeaponType::NONE},
	{"SWORD", WeaponType::SWORD},
	{"HAMMER", WeaponType::HAMMER},
	{"AXEnCLUB", WeaponType::AXEnCLUB},
	{"SPEAR", WeaponType::SPEAR},
	{"PISTOL", WeaponType::PISTOL},
	{"RIFLE", WeaponType::RIFLE},
	{"SNIPER", WeaponType::SNIPER},
	{"SHOTGUN", WeaponType::SHOTGUN},
	{"ROCKET_LAUNCHER", WeaponType::ROCKET_LAUNCHER},
	{"BOWnCROSSBOW", WeaponType::BOWnCROSSBOW},
	{"STAFF", WeaponType::STAFF}
};

AttackType stringToAttackType(const std::string& str) {
	if (str == "EffectAttack") return AttackType::EFFECT_ATTACK;
	if (str == "Projectile") return AttackType::PROJECTILE;
	if (str == "None") return AttackType::NONE;
}

WeaponType stringToWeaponType(const std::string& typeStr) {
	auto it = weaponTypeMap.find(typeStr);
	if (it != weaponTypeMap.end()) {
		return it->second;
	}
	return WeaponType::NONE;
}

EffectAttackType stringToEffectAttackType(const std::string& str) {
	if (str == "SLASH") return EffectAttackType::SLASH;
	if (str == "LUNGE") return EffectAttackType::LUNGE;
	if (str == "SHOCKWAVE") return EffectAttackType::SHOCKWAVE;
	if (str == "ENERGY_WAVE") return EffectAttackType::ENERGY_WAVE;
	if (str == "LARGE_BOOM") return EffectAttackType::LARGE_BOOM;
	if (str == "MEDIUM_BOOM") return EffectAttackType::MEDIUM_BOOM;
	if (str == "SMALL_BOOM") return EffectAttackType::SMALL_BOOM;
}

StatusEffect stringToStatusEffect(const std::string& str) {
	if (str == "NONE") return StatusEffect::NONE;
	if (str == "BURNS") return StatusEffect::BURNS;
	if (str == "POISON") return StatusEffect::POISON;
	if (str == "ELECTRIC") return StatusEffect::ELECTRIC;
	if (str == "DIZZINESS") return StatusEffect::DIZZINESS;
	if (str == "FROZEN") return StatusEffect::FROZEN;
	if (str == "FATIGUE") return StatusEffect::FATIGUE;
}

std::shared_ptr<ProjectileInfo> createChainedProjectileInfo(const nlohmann::json& json)
{
	auto proj = std::make_shared<ProjectileInfo>();
	proj->imagePath = RESOURCE_DIR + json["bulletImagePath"].get<std::string>();
	proj->size = json["bulletSize"].get<float>();
	proj->damage = json["damage"].get<int>();
	proj->elementalDamage = stringToStatusEffect(json["elementalDamage"].get<std::string>());
	proj->speed = json["bulletSpeed"].get<float>();
	proj->canReboundBySword = json["bulletCanRebound"].get<bool>();
	proj->canTracking = json["bulletCanTracking"].get<bool>();
	proj->isBubble = json["bulletIsBubble"].get<bool>();
	proj->bubbleTrail = json["bubbleTrail"].get<bool>();
	if (proj->bubbleTrail){
		proj->bubbleImagePath = RESOURCE_DIR + json["bubbleImagePath"].get<std::string>();
	}
	proj->chainProjectionNum = json["chainProjectionNum"].get<int>();

	return proj;
}

std::shared_ptr<EffectAttackInfo> createChainedEffectAttackInfo(const nlohmann::json& json)
{
	auto effect = std::make_shared<EffectAttackInfo>();
	effect->size = json["size"].get<float>();
	effect->damage = json["damage"].get<float>();
	effect->elementalDamage = stringToStatusEffect(json["elementalDamage"].get<std::string>());
	effect->canReflectBullet = false;
	effect->canBlockingBullet = false;
	effect->effectType = stringToEffectAttackType(json["effectType"].get<std::string>());
	return effect;
}

namespace WeaponFactory {
	std::shared_ptr<Weapon> createWeapon(int weaponID) {
		try {
			// 讀取武器配置文件
			nlohmann::json weaponData = Factory::readJsonFile("weapon.json");

			// 根據武器 ID 查找對應的武器資料
			for (const auto& weapon : weaponData) {
				if (weapon["ID"] == weaponID) {
					int id = weapon["ID"].get<int>();
					std::string weaponImagePath = RESOURCE_DIR + weapon["weaponImagePath"].get<std::string>();
					std::string name = weapon["name"].get<std::string>();
					auto attackType = stringToAttackType(weapon["attackType"].get<std::string>());
					auto weaponType = stringToWeaponType(weapon["weaponType"].get<std::string>());
					int damage = weapon["damage"].get<int>();
					int energy = weapon["energy"].get<int>();
					float criticalRate = weapon["criticalRate"].get<float>();
					int offset = weapon["offset"].get<int>();
					float attackInterval = weapon["attackInterval"].get<float>();
					float attackInitPositionOffset = weapon["attackPositionOffset"].get<float>();
					StatusEffect elementalDamage = stringToStatusEffect(weapon["elementalDamage"].get<std::string>());
					int dropLevel = weapon["dropLevel"].get<int>();
					int basicPrice = weapon["basicPrice"].get<int>();
					glm::vec2 handOffset = glm::vec2(weapon["handOffset"].at("x").get<float>(), weapon["handOffset"].at("y").get<float>());
					glm::vec2 holdingPosition = glm::vec2(weapon["holdingPosition"].at("x").get<float>(), weapon["holdingPosition"].at("y").get<float>());
					std::shared_ptr<Weapon> weaponPtr;

					// 根據 type 建立不同類型的武器
					if (attackType == AttackType::EFFECT_ATTACK) {
						MeleeWeaponInfo meleeInfo;
						meleeInfo.id = id;
						meleeInfo.imagePath = weaponImagePath;
						meleeInfo.name = name;
						meleeInfo.attackType = attackType;
						meleeInfo.weaponType = weaponType;
						meleeInfo.damage = damage;
						meleeInfo.energy = energy;
						meleeInfo.criticalRate = criticalRate;
						meleeInfo.offset = offset;
						meleeInfo.attackInterval = attackInterval;
						meleeInfo.attackInitPositionOffset = attackInitPositionOffset;
						meleeInfo.dropLevel = dropLevel;
						meleeInfo.basicPrice = basicPrice;
						meleeInfo.attackRange = weapon["attackRange"].get<float>();

						EffectAttackInfo effectInfo;
						effectInfo.size = weapon["attackRange"].get<float>();
						effectInfo.elementalDamage = elementalDamage;
						effectInfo.effectType = stringToEffectAttackType(weapon["EffectAttackType"].get<std::string>());
						if (weapon.contains("chainedAttack"))
						{
							nlohmann::json chainedAttack = weapon["chainedAttack"];
							effectInfo.chainAttack.enabled = true;
							effectInfo.chainAttack.attackType = stringToAttackType(chainedAttack["attackType"].get<std::string>());
							if(effectInfo.chainAttack.attackType == AttackType::PROJECTILE) {
								effectInfo.chainAttack.nextAttackInfo = createChainedProjectileInfo(chainedAttack);
							}else if(effectInfo.chainAttack.attackType == AttackType::EFFECT_ATTACK) {
								effectInfo.chainAttack.nextAttackInfo = createChainedEffectAttackInfo(chainedAttack);
							}
						}
						meleeInfo.defaultEffectAttackInfo = effectInfo;

						weaponPtr =  std::make_shared<MeleeWeapon>(meleeInfo);
					}
					else if (attackType == AttackType::PROJECTILE) {
						GunWeaponInfo gunInfo;
						gunInfo.id = id;
						gunInfo.imagePath = weaponImagePath;
						gunInfo.name = name;
						gunInfo.attackType = attackType;
						gunInfo.weaponType = weaponType;
						gunInfo.damage = damage;
						gunInfo.energy = energy;
						gunInfo.criticalRate = criticalRate;
						gunInfo.offset = offset;
						gunInfo.attackInterval = attackInterval;
						gunInfo.attackInitPositionOffset = attackInitPositionOffset;
						gunInfo.dropLevel = dropLevel;
						gunInfo.basicPrice = basicPrice;

						gunInfo.numOfBullets = weapon["numOfBullets"].get<int>();
						gunInfo.bulletCanTracking = weapon["bulletCanTracking"].get<bool>();

						ProjectileInfo projectileInfo;
						projectileInfo.imagePath = RESOURCE_DIR + weapon["bulletImagePath"].get<std::string>();
						projectileInfo.size = weapon["bulletSize"].get<float>();
						projectileInfo.elementalDamage = elementalDamage;
						projectileInfo.speed = weapon["bulletSpeed"].get<float>();
						projectileInfo.canReboundBySword = weapon["bulletCanRebound"].get<bool>();
						projectileInfo.canTracking = weapon["bulletCanTracking"].get<bool>();
						projectileInfo.isBubble = weapon["bulletIsBubble"].get<bool>();
						projectileInfo.bubbleTrail = weapon["bubbleTrail"].get<bool>();
						if (projectileInfo.bubbleTrail){
							projectileInfo.bubbleImagePath = RESOURCE_DIR + weapon["bubbleImagePath"].get<std::string>();
						}
						if (weapon.contains("chainedAttack"))
						{
							nlohmann::json chainedAttack = weapon["chainedAttack"];
							projectileInfo.chainAttack.enabled = true;
							projectileInfo.chainAttack.attackType = stringToAttackType(chainedAttack["attackType"].get<std::string>());
							if(projectileInfo.chainAttack.attackType == AttackType::PROJECTILE) {
								projectileInfo.chainAttack.nextAttackInfo = createChainedProjectileInfo(chainedAttack);
							}else if(projectileInfo.chainAttack.attackType == AttackType::EFFECT_ATTACK) {
								projectileInfo.chainAttack.nextAttackInfo = createChainedEffectAttackInfo(chainedAttack);
							}
						}
						gunInfo.defaultProjectileInfo = projectileInfo;

						weaponPtr = std::make_shared<GunWeapon>(gunInfo);
					}
					auto followerComp = weaponPtr->AddComponent<FollowerComponent>(ComponentType::FOLLOWER);
					followerComp->SetHandOffset(handOffset);
					followerComp->SetHoldingPosition(holdingPosition);
					followerComp->SetZIndexOffset(0.5f);
					if(Weapon::weaponHasOffset(attackType, weaponType)){
						followerComp->SetIsSword(true);
					}
					// LOG_DEBUG("create Weapon successful");

					return weaponPtr;
				}
			}
		}
		catch (const std::exception& e) {
			LOG_ERROR("Failed to create weapon {}: {}", weaponID, e.what());
			return nullptr;  // 如果找不到對應的武器，返回 nullptr
		}
	}
}




