//
// Created by tjx20 on 3/10/2025.
//

#include "Factory/WeaponFactory.hpp"

 WeaponFactory::WeaponFactory() {  }

std::shared_ptr<Weapon> WeaponFactory::createWeapon(const int weaponID) {
 	// 讀取武器配置文件
 	nlohmann::json weaponData = readJsonFile("weapon.json");

 	// 根據武器 ID 查找對應的武器資料
 	for (const auto& weapon : weaponData) {
 		if (weapon["ID"] == weaponID) {
 			std::string type = weapon["weaponType"];
 			std::string weaponImagePath = RESOURCE_DIR + weapon["weaponImagePath"].get<std::string>();
			std::string bulletImagePath = RESOURCE_DIR + weapon["bulletImagePath"].get<std::string>();
 			std::string name = weapon["name"];
 			int damage = weapon["damage"];
 			int energy = weapon["energy"];
 			float criticalRate = weapon["criticalRate"];
 			int offset = weapon["offset"];
 			float attackSpeed = weapon["attackSpeed"];

 			// 根據 type 建立不同類型的武器
 			if (type == "Melee") {
 				float range = weapon["attackRange"];
 				return std::make_shared<MeleeWeapon>(weaponImagePath, bulletImagePath, name, damage, energy, criticalRate, offset, attackSpeed, range);
 			}
 			else if (type == "Gun") {
 				float size = weapon["bulletSize"];
 				return std::make_shared<GunWeapon>(weaponImagePath, bulletImagePath, name, damage, energy, criticalRate, offset, attackSpeed, size);
 			}
 			// else if (type == "Bow") {
 			// 	float chargeTime = weapon["chargeTime"];
 			// 	return std::make_shared<Bow>(name, damage, energy, criticalRate, attackSpeed, chargeTime);
 			// }
 			// else if (type == "LaserGun") {
 			// 	float beamDuration = weapon["beamDuration"];
 			// 	return std::make_shared<LaserGun>(name, damage, energy, criticalRate, attackSpeed, beamDuration);
 			// }
 			else {
 				LOG_DEBUG("Unknown weapon type: {}", type);
 				return nullptr;
 			}
 		}
 	}

 	LOG_DEBUG("Weapon not found: {}", weaponID);
 	return nullptr;  // 如果找不到對應的武器，返回 nullptr
 }

std::shared_ptr<Weapon> WeaponFactory::createRandomWeapon() {
	return nullptr;
}

