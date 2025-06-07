//
// Created by tjx20 on 3/10/2025.
//

#include "Factory/CharacterFactory.hpp"
#include "Factory/WeaponFactory.hpp"

#include "Animation.hpp"
#include "Skill/FullFirepower.hpp"

#include "Components/AiComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/EnemyAI/AttackStrategy.hpp"
#include "Components/EnemyAI/MoveStrategy.hpp"
#include "Components/EnemyAI/UtilityStrategy.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/SkillComponent.hpp"
#include "Components/TalentComponet.hpp"
#include "Components/WalletComponent.hpp"

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
	if (stateStr == "SKILL2") return State::SKILL2;
	if (stateStr == "SKILL3") return State::SKILL3;
	if (stateStr == "SKILL4") return State::SKILL4;
	if (stateStr == "SKILL5") return State::SKILL5;
	if (stateStr == "ATTACK") return State::ATTACK;
	if (stateStr == "DEAD") return State::DEAD;
	throw std::invalid_argument("Unknown state: " + stateStr);
}

std::unordered_map<State, std::shared_ptr<Animation>> parseCharacterAnimations(const nlohmann::json& animationsJson) {
	std::unordered_map<State, std::shared_ptr<Animation>> animations;
	for (const auto& [key, value] : animationsJson.items()) {
		State state = stringToState(key);
		std::vector<std::string> frames;

		if (value.is_object() && value.contains("path")) {
			float interval = 100.0f;
			bool needLoop = false;

			for (const auto& frame : value["path"]) {
				frames.push_back(RESOURCE_DIR + frame.get<std::string>());
			}

			if (value.contains("FPS")) {
				const auto fps = value["FPS"].get<int>();
				interval = 1000.0f / fps;
			}

			if (value.contains("Loop")) {
				needLoop = value["Loop"].get<bool>();
			}

			animations[state] = std::make_shared<Animation>(frames, needLoop, interval);
		} else if (value.is_array()) {
			for (const auto& frame : value) {
				frames.push_back(RESOURCE_DIR + frame.get<std::string>());
			}
			animations[state] = std::make_shared<Animation>(frames, true);
		} else {
			throw std::invalid_argument("Unknown frames");
		}
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
			std::string name = characterInfo["name"].get<std::string>();
			auto player = std::make_shared<Character>(name, type);
			player->SetZIndexType(ZIndexType::OBJECTHIGH); // 設置對應的ZIndexLayer

			auto animation = parseCharacterAnimations(characterInfo["animations"]);
			int maxHp = characterInfo["maxHp"].get<int>();
			int maxArmor = characterInfo["maxArmor"].get<int>();
			int maxEnergy = characterInfo["maxEnergy"].get<int>();

			float moveSpeed = characterInfo["speed"].get<float>();

			// 解析武器名稱並創建武器
			const int weaponID = characterInfo["weaponID"].get<int>();
			auto weapon = WeaponFactory::createWeapon(weaponID);

			double criticalRate = characterInfo["criticalRate"].get<double>();
			int handBladeDamage = characterInfo["handBladeDamage"].get<int>();

			// 讀取技能
			auto skill = createSkill(player, characterInfo["skill"]);

			auto animationComponent = player->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComponent = player->AddComponent<StateComponent>(ComponentType::STATE);
			auto healthComponent = player->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, maxArmor, maxEnergy);
			auto movementComponent = player->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
			auto inputComponent = player->AddComponent<InputComponent>(ComponentType::INPUT);
			auto attackComponent = player->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, criticalRate, handBladeDamage, 0);
			auto skillComponent = player->AddComponent<SkillComponent>(ComponentType::SKILL, skill);
			auto flickerComponent = player->AddComponent<FlickerComponent>(ComponentType::FLICKER);
			auto walletComp = player->AddComponent<WalletComponent>(ComponentType::WALLET);
			auto talentComp = player->AddComponent<TalentComponent>(ComponentType::TALENT);
			auto CollisionComp = player->AddComponent<CollisionComponent>(ComponentType::COLLISION);
			CollisionComp->SetCollisionLayer(CollisionLayers_Player);
			CollisionComp->AddCollisionMask(CollisionLayers_Terrain);
			CollisionComp->AddCollisionMask(CollisionLayers_Enemy);
			CollisionComp->AddCollisionMask(CollisionLayers_Enemy_Projectile);
			CollisionComp->AddCollisionMask(CollisionLayers_Enemy_EffectAttack);
			CollisionComp->SetSize(glm::vec2(16.0f));
			CollisionComp->SetOffset(glm::vec2(6.0f,-6.0f));
			auto FollowerComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER);
			FollowerComp->SetFollower(player);
			FollowerComp->Update(); // 直接更新一次位置
			//FollowerComp->SetTargetMouse(true);

			auto weapon2 = WeaponFactory::createWeapon(2);
			attackComponent->AddWeapon(weapon2);
			LOG_DEBUG("Player created");
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
	if (stateStr == "Boss") return MonsterType::BOSS;
}

std::unordered_map<AttackStrategies, std::shared_ptr<IAttackStrategy>> stringToAtkStrategies(const nlohmann::json& atkStrategies){
	std::unordered_map<AttackStrategies, std::shared_ptr<IAttackStrategy>> strategies;
	for (const auto& atkType : atkStrategies) {
		if(atkType == "Collision") strategies[AttackStrategies::COLLISION_ATTACK] = std::make_shared<CollisionAttack>();
		if(atkType == "Melee") strategies[AttackStrategies::MELEE] = std::make_shared<MeleeAttack>();
		if(atkType == "Gun") strategies[AttackStrategies::GUN] = std::make_shared<GunAttack>();
		if(atkType == "Boss") strategies[AttackStrategies::BOSS] = std::make_shared<BossAttackStrategy>();
		if(atkType == "None")  strategies[AttackStrategies::NONE] = std::make_shared<NoAttack>();
	}
	return strategies;
}

bool JsonArrayContains(const nlohmann::json& array, const std::string& target) {
	if (!array.is_array()) return false;

	return std::any_of(array.begin(), array.end(),
		[&target](const auto& item) {
			return item.is_string() && item.template get<std::string>() == target;
		}
	);
}

std::shared_ptr<Character> CharacterFactory::createEnemy(const int id) {
    // 在 JSON 陣列中搜尋符合名稱的角色
    for (const auto& characterInfo : enemyJsonData) {
        if (characterInfo["ID"] == id) {
        	std::string name = characterInfo["name"].get<std::string>();
        	CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
        	std::shared_ptr<Character> enemy = std::make_shared<Character>(name, type);
        	enemy->SetZIndexType(ZIndexType::OBJECTHIGH);

        	//檢查是否是精英形態
        	if(characterInfo["isElite"].get<bool>() == true) enemy->m_Transform.scale = glm::vec2(1.4f);
        	auto animation = parseCharacterAnimations(characterInfo["animations"]);
			MonsterType aiType = stringToMonsterType(characterInfo["monsterType"].get<std::string>());
        	int monsterPoint = characterInfo["monsterPoint"].get<int>();
        	int maxHp = characterInfo["maxHp"].get<int>();
            float moveSpeed = characterInfo["speed"].get<float>();
        	float bodySize = characterInfo["size"].get<float>();
        	std::shared_ptr<Weapon> weapon = nullptr;
        	int collisionDamage = 0;

        	// AIComponent
        	std::shared_ptr<IMoveStrategy> moveStrategy;
        	auto attackStrategies = stringToAtkStrategies(characterInfo["attackType"]);
        	bool isCollisionAttack = JsonArrayContains(characterInfo["attackType"], "Collision");
        	std::shared_ptr<IUtilityStrategy> utilityStrategy = nullptr;
        	if (aiType == MonsterType::SUMMON) {
        		moveStrategy = std::make_shared<NoMove>();
        		utilityStrategy = std::make_shared<SummonUtility>();
        	} else if (aiType == MonsterType::ATTACK) {
        		moveStrategy = std::make_shared<ChaseMove>();
        	} else if (aiType == MonsterType::WANDER) {
        		moveStrategy = std::make_shared<WanderMove>();
        	} else if (aiType == MonsterType::BOSS)
        		moveStrategy = std::make_shared<BossMove>();
        	else LOG_ERROR("{}'s moveType not found", id);

        	// 根據攻擊類型
			int haveWeapon = characterInfo["haveWeapon"].get<int>();
        	if (haveWeapon == 0) {
        		if (isCollisionAttack)
        			collisionDamage = characterInfo["collisionDamage"].get<int>();
        	}
        	else{
        		const int weaponId = characterInfo["weaponId"].get<int>();
        		weapon = WeaponFactory::createWeapon(weaponId);
        		const auto followComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER);
        		followComp->SetFollower(enemy);
        		followComp->Update(); // 直接更新位置
        	}
        	auto animationComp = enemy->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto flickerComp = enemy->AddComponent<FlickerComponent>(ComponentType::FLICKER);
        	auto stateComp = enemy->AddComponent<StateComponent>(ComponentType::STATE);
        	auto healthComp = enemy->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, 0, 0);
        	auto movementComp = enemy->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
        	auto attackComp = enemy->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, 0, 0, collisionDamage);
        	auto aiComp = enemy->AddComponent<AIComponent>(ComponentType::AI, aiType, moveStrategy, attackStrategies, utilityStrategy, monsterPoint);
        	auto collisionComp = enemy->AddComponent<CollisionComponent>(ComponentType::COLLISION);
        	collisionComp->SetCollisionLayer(CollisionLayers_Enemy);
        	if (haveWeapon == 0 && isCollisionAttack) collisionComp->AddCollisionMask(CollisionLayers_Player);
        	collisionComp->AddCollisionMask(CollisionLayers_Terrain);
        	collisionComp->AddCollisionMask(CollisionLayers_Player_Projectile);
        	collisionComp->AddCollisionMask(CollisionLayers_Player_EffectAttack);
        	collisionComp->SetSize(glm::vec2(bodySize));
        	collisionComp->SetOffset(glm::vec2(0.0f,-6.0f));

        	return enemy;
        }
    }

    LOG_ERROR("{}'s ID not found: {}", id);
	return nullptr;
}