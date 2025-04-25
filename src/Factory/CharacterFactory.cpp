//
// Created by tjx20 on 3/10/2025.
//

#include "Factory/CharacterFactory.hpp"
#include "Factory/WeaponFactory.hpp"

#include "Animation.hpp"
#include "Skill/FullFirepower.hpp"

#include "Components/AiComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/SkillComponent.hpp"
#include "Components/TalentComponet.hpp"
#include "Components/EnemyAI/MoveStrategy.hpp"
#include "Components/EnemyAI/AttackStrategy.hpp"
#include "Components/EnemyAI/UtilityStrategy.hpp"

#include "Util/Logger.hpp"

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
	if (stateStr == "ATTACK") return State::ATTACK;
	if (stateStr == "DEAD") return State::DEAD;
	throw std::invalid_argument("Unknown state: " + stateStr);
}

std::unordered_map<State, std::shared_ptr<Animation>> parseCharacterAnimations(const nlohmann::json& animationsJson) {
	std::unordered_map<State, std::shared_ptr<Animation>> animations;
	for (const auto& [key, value] : animationsJson.items()) {
		State state = stringToState(key);
		std::vector<std::string> frames;
		for (const auto& frame : value) {
			frames.push_back(RESOURCE_DIR + frame.get<std::string>());
		}
		animations[state] = std::make_shared<Animation>(frames, true); // 使用 shared_ptr 包装 Animation
	}
	return animations;
}

// ================================== (Player) ========================================= //
std::shared_ptr<Skill> createSkill(std::weak_ptr<Character> owner, const nlohmann::json& skillInfo) {
	if (!skillInfo.contains("name"))
	{
		LOG_ERROR("No skill name");
		return nullptr;
	}

	auto skillName = skillInfo["name"].get<std::string>();
	auto iconPath = skillInfo["iconPath"].get<std::string>();
	float duration = skillInfo["durationTime"].get<float>();
	float cooldown = skillInfo["cooldownTime"].get<float>();

	// 根據技能名稱創建對應的技能
	if (skillName == "Full Firepower") {
		return std::make_shared<FullFirepower>(owner, skillName, iconPath, duration, cooldown);
	}
	// else if (skillName == "") {
	//     return std::make_shared<>(...);
	// }

	return nullptr;
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
			player->SetZIndexType(ZIndexType::OBJECTHIGH); // 設置對應的ZIndexLayer

			auto animation = parseCharacterAnimations(characterInfo["animations"]);
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
			auto skill = createSkill(player, characterInfo["skill"]);

			auto animationComponent = player->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComponent = player->AddComponent<StateComponent>(ComponentType::STATE);
			auto healthComponent = player->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, maxArmor, maxEnergy);
			auto movementComponent = player->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
			auto inputComponent = player->AddComponent<InputComponent>(ComponentType::INPUT);
			auto attackComponent = player->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, criticalRate, handBladeDamage, 0);
			auto skillComponent = player->AddComponent<SkillComponent>(ComponentType::SKILL, skill);
			auto tanlentCompoent = player->AddComponent<TalentComponent>(ComponentType::TALENT);
			auto CollisionComp = player->AddComponent<CollisionComponent>(ComponentType::COLLISION);
			CollisionComp->SetCollisionLayer(CollisionLayers_Player);
			CollisionComp->SetCollisionMask(CollisionLayers_Terrain);
			CollisionComp->SetCollisionMask(CollisionLayers_Enemy);
			CollisionComp->SetCollisionMask(CollisionLayers_Enemy_Bullet);
			CollisionComp->SetSize(glm::vec2(16.0f));
			CollisionComp->SetOffset(glm::vec2(6.0f,-6.0f));
			auto FollowerComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER);
			FollowerComp->SetFollower(player);
			FollowerComp->Update(); // 直接更新一次位置
			//FollowerComp->SetTargetMouse(true);
			LOG_DEBUG("Player created");

			auto weapon2 = WeaponFactory::createWeapon(2);
			attackComponent->AddWeapon(weapon2);
			return player;
		}
	}
	LOG_ERROR("{}'s ID not found: {}", id);
	return nullptr;
}

// ================================== (Monster) ========================================= //
MonsterType stringToMonsterType(const std::string& stateStr) {
	if (stateStr == "Attack") return MonsterType::ATTACK;
	if (stateStr == "Summon") return MonsterType::SUMMON;
	if (stateStr == "Wander") return MonsterType::WANDER;
}

std::unordered_map<AttackType, std::shared_ptr<IAttackStrategy>> stringToAtkStrategies(const nlohmann::json& atkStrategies){
	std::unordered_map<AttackType, std::shared_ptr<IAttackStrategy>> strategies;
	for (const auto& atkType : atkStrategies) {
		if(atkType == "Collision") strategies[AttackType::COLLISION] = std::make_shared<CollisionAttack>();
		if(atkType == "Melee") strategies[AttackType::MELEE] = std::make_shared<MeleeAttack>();
		if(atkType == "Gun") strategies[AttackType::GUN] = std::make_shared<GunAttack>();
		if(atkType == "None")  strategies[AttackType::NONE] = std::make_shared<NoAttack>();
	}
	return strategies;
}

std::shared_ptr<Character> CharacterFactory::createEnemy(const int id) {
    // 在 JSON 陣列中搜尋符合名稱的角色
    for (const auto& characterInfo : enemyJsonData) {
        if (characterInfo["ID"] == id) {
        	std::string name = characterInfo["name"];
        	CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
        	std::shared_ptr<Character> enemy = std::make_shared<Character>(name, type);
        	enemy->SetZIndexType(ZIndexType::OBJECTHIGH);

        	auto animation = parseCharacterAnimations(characterInfo["animations"]);
			MonsterType aiType = stringToMonsterType(characterInfo["monsterType"].get<std::string>());
        	int monsterPoint = characterInfo["monsterPoint"];
        	int maxHp = characterInfo["maxHp"];
            float moveSpeed = characterInfo["speed"];
        	float bodySize = characterInfo["size"];
        	std::shared_ptr<Weapon> weapon = nullptr;
        	int collisionDamage = 0;

        	// AIComponent
        	std::shared_ptr<IMoveStrategy> moveStrategy;
        	auto attackStrategies = stringToAtkStrategies(characterInfo["attackType"]);
        	std::shared_ptr<IUtilityStrategy> utilityStrategy = nullptr;
        	if (aiType == MonsterType::SUMMON) {
        		moveStrategy = std::make_shared<NoMove>();
        		utilityStrategy = std::make_shared<SummonUtility>();
        	} else if (aiType == MonsterType::ATTACK) {
        		moveStrategy = std::make_shared<ChaseMove>();
        	} else if (aiType == MonsterType::WANDER) {
        		moveStrategy = std::make_shared<WanderMove>();
        	}
        	else LOG_ERROR("{}'s attackType not found", id);

        	// 根據攻擊類型
			int haveWeapon = characterInfo["haveWeapon"];
        	if (haveWeapon == 0) {
        		collisionDamage = characterInfo["collisionDamage"];
        	}
        	else{
        		const int weaponId = characterInfo["weaponId"];
        		weapon = WeaponFactory::createWeapon(weaponId);
        		const auto followComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER);
        		followComp->SetFollower(enemy);
        		followComp->Update(); // 直接更新位置
        	}
        	auto animationComp = enemy->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComp = enemy->AddComponent<StateComponent>(ComponentType::STATE);
        	auto healthComp = enemy->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, 0, 0);
        	auto movementComp = enemy->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
        	auto attackComp = enemy->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, 0, 0, collisionDamage);
        	auto aiComp = enemy->AddComponent<AIComponent>(ComponentType::AI, aiType, moveStrategy, attackStrategies, utilityStrategy, monsterPoint);
        	auto collisionComp = enemy->AddComponent<CollisionComponent>(ComponentType::COLLISION);
        	collisionComp->SetCollisionLayer(CollisionLayers_Enemy);
        	if (haveWeapon == 0) collisionComp->SetCollisionMask(CollisionLayers_Player);
        	collisionComp->SetCollisionMask(CollisionLayers_Terrain);
        	collisionComp->SetCollisionMask(CollisionLayers_Player_Bullet);
        	collisionComp->SetSize(glm::vec2(bodySize));
        	collisionComp->SetOffset(glm::vec2(6.0f,-6.0f));

        	return enemy;
        }
    }

    LOG_ERROR("{}'s ID not found: {}", id);
	return nullptr;
}