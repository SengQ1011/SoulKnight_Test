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

std::unique_ptr<Weapon> CharacterFactory::createWeaponFromJson(const std::string& weaponName) {
    // 讀取武器配置文件
    nlohmann::json weaponData = readJsonFile("weapon.json");

    // 根據武器名稱查找對應的武器資料
    for (const auto& weapon : weaponData) {
        if (weapon["name"] == weaponName) {
            std::string name = weapon["name"];
            int damage = weapon["damage"];
            float range = weapon["range"];
            int attackSpeed = weapon["attackSpeed"];
            int size = weapon["size"];

            // 創建並返回武器
            return std::make_unique<Weapon>(name, damage, range, attackSpeed, size);
        }
    }
    LOG_DEBUG("Weapon not found: {}", weaponName);
    return nullptr;  // 如果找不到對應的武器，返回 nullptr
}

std::unique_ptr<Character> CharacterFactory::createCharacterFromJson(const std::string& type, const int id) {
    // 讀取角色 JSON 資料
	nlohmann::json characterData;
	if (type == "player") {
		characterData = readJsonFile("json/player.json");
	}
	else if (type == "enemy")
	{
		characterData = readJsonFile("json/enemy.json");
	}
	else
	{
		LOG_DEBUG("Error: Unknown type of character: {}", type);
		return nullptr;
	}


    // 在 JSON 陣列中搜尋符合名稱的角色
    for (const auto& characterInfo : characterData) {
        if (characterInfo["id"] == id) {
            std::string imagePath = characterInfo["imagePath"];
            int maxHp = characterInfo["maxHp"];
            float moveSpeed = characterInfo["speed"];
            int aimRange = characterInfo["aimRange"];
            float collisionWidth = characterInfo["collisionBox"]["width"];
            float collisionHeight = characterInfo["collisionBox"]["height"];

            // 解析武器名稱並創建武器
            std::string weaponName = characterInfo["weapon"];
            auto weapon = createWeaponFromJson(weaponName);

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

            // 根據 JSON 內容來決定創建 Player 或 Enemy
            if (type == "player") {
                return std::make_unique<Player>(imagePath, maxHp, moveSpeed, aimRange, collisionWidth, collisionHeight, std::move(weapon),
                                                maxArmor, maxEnergy, criticalRate, handBladeDamage, skill);
            }
            else if (type == "enemy") {
                return std::make_unique<Enemy>(imagePath, maxHp, moveSpeed, aimRange, collisionWidth, collisionHeight, std::move(weapon));
            }
        }
    }

    LOG_DEBUG("{}'s ID not found: {}", type, id);
    return nullptr;  // 找不到角色
}