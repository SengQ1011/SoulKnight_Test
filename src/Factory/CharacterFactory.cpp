//
// Created by tjx20 on 3/10/2025.
//

#include "Factory/CharacterFactory.hpp"
#include <fstream>
#include <memory>

#include "Components/AttackComponent.hpp"
#include "Components/InputComponent.hpp"

CharacterFactory::CharacterFactory() {}

CharacterType stringToCharacterType(const std::string& typeStr) {
	static const std::unordered_map<std::string, CharacterType> typeMap = {
		{"PLAYER", CharacterType::PLAYER},
		{"ENEMY", CharacterType::ENEMY},
		{"NPC", CharacterType::NPC}
	};

	auto it = typeMap.find(typeStr);
	if (it != typeMap.end()) {
		return it->second;
	}

	// 如果找不到匹配的类型，可以抛出异常或返回默认值
	throw std::runtime_error("Unknown character type: " + typeStr);
	// 或者 return CharacterType::PLAYER; // 默认值
}

State stringToState(const std::string& stateStr) {
	if (stateStr == "STANDING") return State::STANDING;
	if (stateStr == "MOVING") return State::MOVING;
	if (stateStr == "SKILL") return State::SKILL;
	if (stateStr == "DEAD") return State::DEAD;
	throw std::invalid_argument("Unknown state: " + stateStr);
}

std::unordered_map<State, std::shared_ptr<Animation>> parseAnimations(const nlohmann::json& animationsJson) {
	std::unordered_map<State, std::shared_ptr<Animation>> animations;
	for (const auto& [key, value] : animationsJson.items()) {
		State state = stringToState(key);
		std::vector<std::string> frames;
		for (const auto& frame : value) {
			frames.push_back(RESOURCE_DIR + frame.get<std::string>());
		}
		animations[state] = std::make_shared<Animation>(frames); // 使用 shared_ptr 包装 Animation
	}
	return animations;
}

std::shared_ptr<Character> CharacterFactory::createPlayer(const int id) {
	// 讀取角色 JSON 資料
	nlohmann::json characterData = readJsonFile("player.json");

	// 在 JSON 陣列中搜尋符合名稱的角色
	for (const auto& characterInfo : characterData) {
		if (characterInfo["ID"] == id) {
			CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
			std::string name = characterInfo["name"];
			auto animation = parseAnimations(characterInfo["animations"]);
			int maxHp = characterInfo["maxHp"];
			int maxArmor = characterInfo["maxArmor"];
			int maxEnergy = characterInfo["maxEnergy"];

			float moveSpeed = characterInfo["speed"];

			// 解析武器名稱並創建武器
			const int weaponID = characterInfo["weaponID"];
			auto weapon = wf.createWeapon(weaponID);

			double criticalRate = characterInfo["criticalRate"];
			int handBladeDamage = characterInfo["handBladeDamage"];

			// 讀取技能
			// std::shared_ptr<Skill> skill = nullptr;
			// if (characterInfo.contains("skill")) {
			// 	skill = std::make_shared<Skill>(
			// 		characterInfo["skill"]["name"],
			// 		characterInfo["skill"]["cooldown"],
			// 		characterInfo["skill"]["damage"]
			// 	);
			// }

			// 根據 JSON 內容來決定創建 Player
			auto player = std::make_shared<Character>(name, type);

			auto healthComponent = player->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, maxArmor, maxEnergy);
			auto animationComponent = player->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComponent = player->AddComponent<StateComponent>(ComponentType::STATE);
			auto inputComponent = player->AddComponent<InputComponent>(ComponentType::INPUT);
			auto attackComponent = player->AddComponent<AttackComponent>(ComponentType::ATTACK, criticalRate, handBladeDamage, weapon);
			auto CollisionComp = player->AddComponent<CollisionComponent>(ComponentType::COLLISION);
			CollisionComp->SetCollisionLayer(CollisionLayers_Player);
			CollisionComp->SetCollisionMask(CollisionLayers_Terrain);
			CollisionComp->SetCollisionMask(CollisionLayers_Enemy);
			CollisionComp->SetSize(glm::vec2(16.0f));
			CollisionComp->SetOffset(glm::vec2(-4.0f,-13.0f));
			auto FollowerComp = weapon->AddComponent<FollowerComponent>(ComponentType::FOLLOWER);
			FollowerComp->SetFollower(player);
			FollowerComp->IsTargetMouse(true);
			FollowerComp->SetHandOffset(glm::vec2(30/7.0f,-25/4.0f));
			FollowerComp->SetHoldingPosition(glm::vec2(30/2.0f,0));
			auto movementComponent = player->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
			LOG_DEBUG("Player created");
			return player;
		}
	}
	LOG_DEBUG("{}'s ID not found: {}", id);
	return nullptr;
}

std::shared_ptr<Character>CharacterFactory::createEnemy(const int id) {
    // 讀取角色 JSON 資料
	nlohmann::json characterData = readJsonFile("enemy.json");

    // 在 JSON 陣列中搜尋符合名稱的角色
    for (const auto& characterInfo : characterData) {
        if (characterInfo["id"] == id) {
        	std::string name = characterInfo["name"];
        	CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
        	auto animation = parseAnimations(characterInfo["animations"]);
            int maxHp = characterInfo["maxHp"];
            float moveSpeed = characterInfo["speed"];
            int aimRange = characterInfo["aimRange"];

            // 解析武器名稱並創建武器
            int weaponID = characterInfo["weaponID"];
            auto weapon = wf.createWeapon(weaponID);

            // 根據 JSON 內容來決定創建 Enemy
        	return std::make_shared<Character>(name, type);
        }
    }

    LOG_DEBUG("{}'s ID not found: {}", id);
	return nullptr;
}