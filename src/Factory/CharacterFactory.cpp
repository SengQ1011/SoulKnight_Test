//
// Created by tjx20 on 3/10/2025.
//

#include "Factory/CharacterFactory.hpp"
#include <fstream>
#include <memory>

#include "Components/AttackComponent.hpp"
#include "Components/InputComponent.hpp"

CharacterFactory* CharacterFactory::instance = nullptr;

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

AIType stringToAiType(const std::string& stateStr) {
	if (stateStr == "Attack") return AIType::ATTACK;
	if (stateStr == "Summon") return AIType::SUMMON;
	if (stateStr == "Wander") return AIType::WANDER;
	LOG_ERROR("Unknown AiType: {}", stateStr);
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
			auto player = std::make_shared<Character>(name, type);
			player->SetZIndexType(ZIndexType::OBJECTHIGH);

			auto animation = parseAnimations(characterInfo["animations"]);
			int maxHp = characterInfo["maxHp"];
			int maxArmor = characterInfo["maxArmor"];
			int maxEnergy = characterInfo["maxEnergy"];

			float moveSpeed = characterInfo["speed"];

			// 解析武器名稱並創建武器
			const int weaponID = characterInfo["weaponID"];
			auto weapon = WeaponFactory::createWeapon(weaponID);

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

			auto animationComponent = player->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComponent = player->AddComponent<StateComponent>(ComponentType::STATE);
			auto healthComponent = player->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, maxArmor, maxEnergy);
			auto movementComponent = player->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
			auto inputComponent = player->AddComponent<InputComponent>(ComponentType::INPUT);
			auto attackComponent = player->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, criticalRate, handBladeDamage, 0);
			auto CollisionComp = player->AddComponent<CollisionComponent>(ComponentType::COLLISION);
			CollisionComp->SetCollisionLayer(CollisionLayers_Player);
			CollisionComp->SetCollisionMask(CollisionLayers_Terrain);
			CollisionComp->SetCollisionMask(CollisionLayers_Enemy);
			CollisionComp->SetSize(glm::vec2(16.0f));
			CollisionComp->SetOffset(glm::vec2(6.0f,-6.0f));
			auto FollowerComp = weapon->AddComponent<FollowerComponent>(ComponentType::FOLLOWER);
			FollowerComp->SetFollower(player);
			FollowerComp->IsTargetMouse(true);
			FollowerComp->SetHandOffset(glm::vec2(30/7.0f,-25/4.0f));
			FollowerComp->SetHoldingPosition(glm::vec2(30/2.0f,0));
			LOG_DEBUG("Player created");
			return player;
		}
	}
	LOG_DEBUG("{}'s ID not found: {}", id);
	return nullptr;
}

std::shared_ptr<Character> CharacterFactory::createEnemy(const int id) {
	std::shared_ptr<Character> enemy;

	// 從緩存中檢查
	if (enemyTemplates.count(id))
	{

	}

	// 讀取角色 JSON 資料
	nlohmann::json characterData = readJsonFile("enemy.json");

    // 在 JSON 陣列中搜尋符合名稱的角色
    for (const auto& characterInfo : characterData) {
        if (characterInfo["ID"] == id) {
        	std::string name = characterInfo["name"];
        	CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
        	enemy = std::make_shared<Character>(name, type);
        	enemy->SetZIndexType(ZIndexType::OBJECTHIGH);

        	auto animation = parseAnimations(characterInfo["animations"]);
			AIType aiType = stringToAiType(characterInfo["monsterType"].get<std::string>());
        	int monsterPoint = characterInfo["monsterPoint"];
        	int maxHp = characterInfo["maxHp"];
            float moveSpeed = characterInfo["speed"];
        	std::shared_ptr<Weapon> weapon = nullptr;
        	int collisionDamage = 0;

            // 根據攻擊類型
            std::string attackType = characterInfo["attackType"];
        	if (attackType == "Collision") {
				collisionDamage = characterInfo["collisionDamage"];
        	}
        	else if (attackType == "Gun"|| attackType == "Malee")
        	{
        		// const int weaponID = characterInfo["weaponID"];
        		// weapon = wf.createWeapon(weaponID);
        	}

        	auto animationComp = enemy->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComp = enemy->AddComponent<StateComponent>(ComponentType::STATE);
        	auto healthComp = enemy->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, 0, 0);
        	auto movementComp = enemy->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
        	auto attackComp = enemy->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, 0, 0, collisionDamage);
        	auto aiComp = enemy->AddComponent<AIComponent>(ComponentType::AI, aiType, monsterPoint);
			auto collisionComp = enemy->AddComponent<CollisionComponent>(ComponentType::COLLISION);
        	collisionComp->SetCollisionLayer(CollisionLayers_Enemy);
        	collisionComp->SetCollisionMask(CollisionLayers_Player);
        	collisionComp->SetCollisionMask(CollisionLayers_Terrain);
        	collisionComp->SetSize(glm::vec2(16.0f));
        	collisionComp->SetOffset(glm::vec2(6.0f,-6.0f));

        	return enemy;
        }
    }

    LOG_DEBUG("{}'s ID not found: {}", id);
	return nullptr;
}