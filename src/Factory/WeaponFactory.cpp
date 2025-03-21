//
// Created by tjx20 on 3/10/2025.
//

#include "Factory/WeaponFactory.hpp"

 WeaponFactory::WeaponFactory() {  }

std::shared_ptr<Weapon> WeaponFactory::createWeapon(const int weaponID) {
	// 讀取武器配置文件
	nlohmann::json weaponData = readJsonFile("../json/weapon.json");

	// 根據武器名稱查找對應的武器資料
	for (const auto& weapon : weaponData) {
		if (weapon["ID"] == weaponID) {
			std::string imagePath = RESOURCE_DIR + weapon["imagePath"].get<std::string>();
			std::string name = weapon["name"];
			int damage = weapon["damage"];
			int energy = weapon["energy"];
			float criticalRate = weapon["criticalRate"];
			int offset = weapon["offset"];
			int dropLevel = weapon["dropLevel"];
			float range = weapon["attackRange"];
			float size = weapon["bulletSize"];
			int attackSpeed = weapon["attackSpeed"];

			// 創建並返回武器
			return std::make_shared<Weapon>(imagePath, name, damage, energy, criticalRate, offset, dropLevel, range, size, attackSpeed);
		}
	}
	LOG_DEBUG("Weapon not found: {}", weaponID);
	return nullptr;  // 如果找不到對應的武器，返回 nullptr
}

std::shared_ptr<Weapon> WeaponFactory::createRandomWeapon() {
	return nullptr;
}

