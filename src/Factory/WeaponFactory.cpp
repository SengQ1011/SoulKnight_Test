//
// Created by tjx20 on 3/10/2025.
//
// WeaponFactory.cpp

#include "Factory/WeaponFactory.hpp"
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
	if (str == "Melee") return AttackType::MELEE;
	if (str == "Gun") return AttackType::GUN;
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
}

namespace WeaponFactory {
	std::shared_ptr<Weapon> createWeapon(int weaponID) {
		try {
			// 讀取武器配置文件
			nlohmann::json weaponData = Factory::readJsonFile("weapon.json");

			// 根據武器 ID 查找對應的武器資料
			for (const auto& weapon : weaponData) {
				if (weapon["ID"] == weaponID) {
					std::string weaponImagePath = RESOURCE_DIR + weapon["weaponImagePath"].get<std::string>();
					std::string name = weapon["name"].get<std::string>();
					auto attackType = stringToAttackType(weapon["attackType"].get<std::string>());
					auto weaponType = stringToWeaponType(weapon["weaponType"].get<std::string>());
					int damage = weapon["damage"].get<int>();
					int energy = weapon["energy"].get<int>();
					float criticalRate = weapon["criticalRate"].get<float>();
					int offset = weapon["offset"].get<int>();
					float attackInterval = weapon["attackInterval"].get<float>();
					int dropLevel = weapon["dropLevel"].get<int>();
					int basicPrice = weapon["basicPrice"].get<int>();
					std::shared_ptr<Weapon> weaponPtr;

					// 根據 type 建立不同類型的武器
					if (attackType == AttackType::MELEE) {
						MeleeWeaponInfo meleeInfo;
						meleeInfo.imagePath = weaponImagePath;
						meleeInfo.name = name;
						meleeInfo.attackType = attackType;
						meleeInfo.weaponType = weaponType;
						meleeInfo.damage = damage;
						meleeInfo.energy = energy;
						meleeInfo.criticalRate = criticalRate;
						meleeInfo.offset = offset;
						meleeInfo.attackInterval = attackInterval;
						meleeInfo.dropLevel = dropLevel;
						meleeInfo.basicPrice = basicPrice;
						meleeInfo.attackRange = weapon["attackRange"].get<float>();
						meleeInfo.attackEffectType = stringToEffectAttackType(weapon["EffectAttackType"].get<std::string>());
						weaponPtr =  std::make_shared<MeleeWeapon>(meleeInfo);
					}
					else if (attackType == AttackType::GUN) {
						GunWeaponInfo gunInfo;
						gunInfo.imagePath = weaponImagePath;
						gunInfo.name = name;
						gunInfo.attackType = attackType;
						gunInfo.weaponType = weaponType;
						gunInfo.damage = damage;
						gunInfo.energy = energy;
						gunInfo.criticalRate = criticalRate;
						gunInfo.offset = offset;
						gunInfo.attackInterval = attackInterval;
						gunInfo.dropLevel = dropLevel;
						gunInfo.basicPrice = basicPrice;
						gunInfo.bulletImagePath = RESOURCE_DIR + weapon["bulletImagePath"].get<std::string>();
						gunInfo.bulletSize = weapon["bulletSize"].get<float>();
						gunInfo.bulletSpeed = weapon["bulletSpeed"].get<float>();
						gunInfo.bulletCanReboundBySword = weapon["bulletCanRebound"].get<bool>();
						gunInfo.bulletIsBubble = weapon["bulletIsBubble"].get<bool>();
						gunInfo.haveBubbleTrail = weapon["bubbleTrail"].get<bool>();
						if (gunInfo.haveBubbleTrail){
							gunInfo.bubbleImagePath = RESOURCE_DIR + weapon["bubbleImagePath"].get<std::string>();
							LOG_DEBUG("have trail");
						}
						weaponPtr = std::make_shared<GunWeapon>(gunInfo);
					}
					auto followerComp = weaponPtr->AddComponent<FollowerComponent>(ComponentType::FOLLOWER);
					followerComp->SetHandOffset(glm::vec2(30/7.0f,-25/4.0f));
					followerComp->SetHoldingPosition(glm::vec2(30/2.0f,0));
					followerComp->SetZIndexOffset(0.5f);
					if(Weapon::weaponHasOffset(attackType, weaponType)){
						followerComp->SetIsSword(true);
					}
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




