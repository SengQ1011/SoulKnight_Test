//
// Created by tjx20 on 3/10/2025.
//
// WeaponFactory.cpp

#include "Factory/WeaponFactory.hpp"
#include "Factory/Factory.hpp"

#include "Weapon/GunWeapon.hpp"
#include "Weapon/MeleeWeapon.hpp"

#include "Util/Logger.hpp"

EffectAttackType stringToEffectAttackType(const std::string& stateStr) {
	if (stateStr == "SLASH") return EffectAttackType::SLASH;
	if (stateStr == "LUNGE") return EffectAttackType::LUNGE;
	if (stateStr == "SHOCKWAVE") return EffectAttackType::SHOCKWAVE;
}

namespace WeaponFactory {
	std::shared_ptr<Weapon> createWeapon(int weaponID) {
		try {
			// 讀取武器配置文件
			nlohmann::json weaponData = Factory::readJsonFile("weapon.json");

			// 根據武器 ID 查找對應的武器資料
			for (const auto& weapon : weaponData) {
				if (weapon["ID"] == weaponID) {
					std::string type = weapon["weaponType"];
					std::string weaponImagePath = RESOURCE_DIR + weapon["weaponImagePath"].get<std::string>();

					std::string name = weapon["name"];
					int damage = weapon["damage"];
					int energy = weapon["energy"];
					float criticalRate = weapon["criticalRate"];
					int offset = weapon["offset"];
					float attackInterval = weapon["attackInterval"];
					bool isSword = false;
					std::shared_ptr<Weapon> weaponPtr;

					// 根據 type 建立不同類型的武器
					if (type == "Melee") {
						float range = weapon["attackRange"];
						auto type = stringToEffectAttackType(weapon["EffectAttackType"]);
						if (const int check = weapon["isSword"]; check == 1) {
							isSword = true;
						}
						weaponPtr =  std::make_shared<MeleeWeapon>(weaponImagePath,  name, damage, energy, criticalRate, offset, attackInterval, range, type);
					}
					else if (type == "Gun") {
						std::string bulletImagePath = RESOURCE_DIR + weapon["bulletImagePath"].get<std::string>();
						float size = weapon["bulletSize"];
						float speed = weapon["bulletSpeed"];
						weaponPtr = std::make_shared<GunWeapon>(weaponImagePath, bulletImagePath, name, damage, energy, criticalRate, offset, attackInterval, size, speed);
					}
					// else if (type == "Bow") {
					// 	float chargeTime = weapon["chargeTime"];
					// 	return std::make_shared<Bow>(name, damage, energy, criticalRate, attackSpeed, chargeTime);
					// }
					// else if (type == "LaserGun") {
					// 	float beamDuration = weapon["beamDuration"];
					// 	return std::make_shared<LaserGun>(name, damage, energy, criticalRate, attackSpeed, beamDuration);
					// }

					auto followerComp = weaponPtr->AddComponent<FollowerComponent>(ComponentType::FOLLOWER);
					followerComp->SetHandOffset(glm::vec2(30/7.0f,-25/4.0f));
					followerComp->SetHoldingPosition(glm::vec2(30/2.0f,0));
					followerComp->SetZIndexOffset(0.5f);
					if(isSword){
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




