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

std::unique_ptr<Character> CharacterFactory::createCharacterFromJson(const std::string& characterJsonFilePath) {
	// 讀取角色配置文件（player.json 或 enemy.json）
	nlohmann::json characterData = readJsonFile(characterJsonFilePath);
	auto characterInfo = characterData[0];  // 假設 JSON 中只有一個角色資料

	std::string characterType = characterInfo["name"];
	std::string imagePath = characterInfo["imagePath"];
	int maxHp = characterInfo["maxHp"];
	float moveSpeed = characterInfo["speed"];
	int aimRange = characterInfo["aimRange"];

	// 解析武器名稱並創建武器
	std::string weaponName = characterInfo["weapon"];
	auto weapon = createWeaponFromJson(weaponName);

	int maxArmor = characterInfo["maxArmor"];
	int maxEnergy = characterInfo["maxEnergy"];
	double criticalRate = characterInfo["criticalRate"];
	int handBladeDamage = characterInfo["handBladeDamage"];
	std::shared_ptr<Skill> skill = nullptr;  // 根據需要從 JSON 中讀取技能資料

    // 創建 Player 或 Enemy
    if (characterType == "Player") {
        return std::make_unique<Player>(imagePath, maxHp, moveSpeed, aimRange, collisionRadius, std::move(weapon),
                                        maxArmor, maxEnergy, criticalRate, handBladeDamage, skill);
    }
    else if (characterType == "Enemy") {
        return std::make_unique<Enemy>(imagePath, maxHp, moveSpeed, aimRange, collisionRadius, std::move(weapon));
    }

    return nullptr;  // 如果沒有匹配的角色類型
}
