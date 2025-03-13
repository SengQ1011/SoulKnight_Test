//
// Created by tjx20 on 3/10/2025.
//

#include "Creature/CharacterFactory.hpp"
#include <memory>
#include <fstream>
#include "Util/Logger.hpp"

nlohmann::json CharacterFactory::readJsonFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        LOG_DEBUG("Error: Unable to open file: {}",filePath);
        return nlohmann::json();  // 如果文件打開失敗，返回空的 JSON 物件
    }

    nlohmann::json jsonData;
    file >> jsonData;
    return jsonData;
}

std::shared_ptr<Weapon> CharacterFactory::createWeapon(const int weaponID) {
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

std::shared_ptr<Player> CharacterFactory::createPlayer(const int id) {
	// 讀取角色 JSON 資料
	nlohmann::json characterData = readJsonFile("../json/player.json");

	// 在 JSON 陣列中搜尋符合名稱的角色
	for (const auto& characterInfo : characterData) {
		if (characterInfo["ID"] == id) {
			std::string imagePath = RESOURCE_DIR + characterInfo["imagePath"].get<std::string>();
			int maxHp = characterInfo["maxHp"];
			float moveSpeed = characterInfo["speed"];
			int aimRange = characterInfo["aimRange"];

			// **讀取碰撞箱資訊**
			float collisionWidth = characterInfo["collisionBox"]["width"];
			float collisionHeight = characterInfo["collisionBox"]["height"];
			float offsetX = characterInfo["collisionBox"]["offsetX"];
			float offsetY = characterInfo["collisionBox"]["offsetY"];
			auto collisionBox = std::make_unique<CollisionBox>(offsetX, offsetY, collisionWidth, collisionHeight);

			// 解析武器名稱並創建武器
			int weaponID = characterInfo["weaponID"];
			auto weapon = createWeapon(weaponID);


			int maxArmor = characterInfo["maxArmor"];
			int maxEnergy = characterInfo["maxEnergy"];
			double criticalRate = characterInfo["criticalRate"];
			int handBladeDamage = characterInfo["handBladeDamage"];

			// 讀取技能
			std::shared_ptr<Skill> skill = nullptr;
			if (characterInfo.contains("skill")) {
				skill = std::make_shared<Skill>(
					characterInfo["skill"]["name"],
					characterInfo["skill"]["cooldown"],
					characterInfo["skill"]["damage"]
				);
			}

			// 根據 JSON 內容來決定創建 Player
			return std::make_shared<Player>(imagePath, maxHp, moveSpeed, aimRange, std::move(collisionBox), weapon,
												maxArmor, maxEnergy, criticalRate, handBladeDamage, skill);
		}
	}
	LOG_DEBUG("{}'s ID not found: {}", id);
	return nullptr;
}

std::shared_ptr<Enemy>CharacterFactory::createEnemy(const int id) {
    // 讀取角色 JSON 資料
	nlohmann::json characterData = readJsonFile("../json/enemy.json");

    // 在 JSON 陣列中搜尋符合名稱的角色
    for (const auto& characterInfo : characterData) {
        if (characterInfo["id"] == id) {
            std::string imagePath = RESOURCE_DIR + characterInfo["imagePath"].get<std::string>();
            int maxHp = characterInfo["maxHp"];
            float moveSpeed = characterInfo["speed"];
            int aimRange = characterInfo["aimRange"];

        	// **讀取碰撞箱資訊**
        	float collisionWidth = characterInfo["collisionBox"]["width"];
        	float collisionHeight = characterInfo["collisionBox"]["height"];
        	float offsetX = characterInfo["collisionBox"]["offsetX"];
        	float offsetY = characterInfo["collisionBox"]["offsetY"];
        	auto collisionBox = std::make_unique<CollisionBox>(offsetX, offsetY, collisionWidth, collisionHeight);

            // 解析武器名稱並創建武器
            int weaponID = characterInfo["weaponID"];
            auto weapon = createWeapon(weaponID);

            // 根據 JSON 內容來決定創建 Enemy
        	return std::make_shared<Enemy>(imagePath, maxHp, moveSpeed, aimRange, std::move(collisionBox), weapon);
        }
    }

    LOG_DEBUG("{}'s ID not found: {}", id);
	return nullptr;
}