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
#include "Components/NPCComponent.hpp"
#include "Components/SkillComponent.hpp"
#include "Components/TalentComponet.hpp"
#include "Components/WalletComponent.hpp"

#include "Util/Logger.hpp"

CharacterFactory *CharacterFactory::instance = nullptr;

CharacterType stringToCharacterType(const std::string &typeStr)
{
	static const std::unordered_map<std::string, CharacterType> typeMap = {
		{"PLAYER", CharacterType::PLAYER}, {"ENEMY", CharacterType::ENEMY}, {"NPC", CharacterType::NPC}};

	auto it = typeMap.find(typeStr);
	if (it != typeMap.end())
	{
		return it->second;
	}

	// 如果找不到匹配的类型，可以抛出异常或返回默认值
	throw std::runtime_error("Unknown character type: " + typeStr);
	// 或者 return CharacterType::PLAYER; // 默认值
}

State stringToState(const std::string &stateStr)
{
	if (stateStr == "STANDING")
		return State::STANDING;
	if (stateStr == "MOVING")
		return State::MOVING;
	if (stateStr == "SKILL")
		return State::SKILL;
	if (stateStr == "SKILL2")
		return State::SKILL2;
	if (stateStr == "SKILL3")
		return State::SKILL3;
	if (stateStr == "SKILL4")
		return State::SKILL4;
	if (stateStr == "SKILL5")
		return State::SKILL5;
	if (stateStr == "ATTACK")
		return State::ATTACK;
	if (stateStr == "DEAD")
		return State::DEAD;
	throw std::invalid_argument("Unknown state: " + stateStr);
}

std::shared_ptr<Animation> createAnimationFromConfig(const nlohmann::json& animationConfig) {
	std::vector<std::string> frames;
	float interval = 100.0f;
	bool needLoop = false;

	if (animationConfig.is_object() && animationConfig.contains("path")) {
		for (const auto& frame : animationConfig["path"]) {
			frames.push_back(RESOURCE_DIR + frame.get<std::string>());
		}

		if (animationConfig.contains("FPS")) {
			const auto fps = animationConfig["FPS"].get<int>();
			interval = 1000.0f / fps;
		}

		if (animationConfig.contains("Loop")) {
			needLoop = animationConfig["Loop"].get<bool>();
		}
	}
	else if (animationConfig.is_array()) {
		for (const auto& frame : animationConfig) {
			frames.push_back(RESOURCE_DIR + frame.get<std::string>());
		}
		needLoop = true;
	}
	else {
		throw std::invalid_argument("Unknown animation format");
	}

	return std::make_shared<Animation>(frames, needLoop, interval, "Animation");
}

std::unordered_map<State, std::shared_ptr<Animation>> parseCharacterAnimations(const nlohmann::json& animationsJson) {
	std::unordered_map<State, std::shared_ptr<Animation>> animations;
	for (const auto& [key, value] : animationsJson.items()) {
		State state = stringToState(key);
		animations[state] = createAnimationFromConfig(value);
	}
	return animations;
}

// ================================== (Player) ========================================= //
std::shared_ptr<Skill> createSkill(std::weak_ptr<Character> owner, const nlohmann::json &skillInfo)
{
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
	if (skillName == "Full Firepower")
	{
		return std::make_shared<FullFirepower>(owner, skillName, iconPath, duration, cooldown);
	}
	// else if (skillName == "") {
	//     return std::make_shared<>(...);
	// }

	return nullptr;
}

std::shared_ptr<Character> CharacterFactory::createPlayer(const int id)
{
	// 讀取角色 JSON 資料
	nlohmann::json characterData = readJsonFile("player.json");

	// 在 JSON 陣列中搜尋符合名稱的角色
	for (const auto &characterInfo : characterData)
	{
		if (characterInfo["ID"] == id)
		{
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
			auto healthComponent =
				player->AddComponent<HealthComponent>(ComponentType::HEALTH, maxHp, maxArmor, maxEnergy);
			auto movementComponent = player->AddComponent<MovementComponent>(ComponentType::MOVEMENT, moveSpeed);
			auto inputComponent = player->AddComponent<InputComponent>(ComponentType::INPUT);
			auto attackComponent =
				player->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, criticalRate, handBladeDamage, 0);
			auto skillComponent = player->AddComponent<SkillComponent>(ComponentType::SKILL, skill);
			auto flickerComponent = player->AddComponent<FlickerComponent>(ComponentType::FLICKER);
			auto walletComp = player->AddComponent<walletComponent>(ComponentType::WALLET);
			auto talentComp = player->AddComponent<TalentComponent>(ComponentType::TALENT);
			auto CollisionComp = player->AddComponent<CollisionComponent>(ComponentType::COLLISION);
			CollisionComp->SetCollisionLayer(CollisionLayers_Player);
			CollisionComp->AddCollisionMask(CollisionLayers_Terrain);
			CollisionComp->AddCollisionMask(CollisionLayers_DestructibleTerrain);
			CollisionComp->AddCollisionMask(CollisionLayers_Enemy);
			CollisionComp->AddCollisionMask(CollisionLayers_Enemy_Projectile);
			CollisionComp->AddCollisionMask(CollisionLayers_Enemy_EffectAttack);
			CollisionComp->SetSize(glm::vec2(16.0f));
			CollisionComp->SetOffset(glm::vec2(6.0f, -6.0f));
			auto FollowerComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER);
			FollowerComp->SetFollower(player);
			FollowerComp->Update(); // 直接更新一次位置
			// FollowerComp->SetTargetMouse(true);

			// auto weapon2 = WeaponFactory::createWeapon(5);
			// attackComponent->AddWeapon(weapon2);
			LOG_DEBUG("Player created");
			return player;
		}
	}
	LOG_ERROR("{}'s ID not found: {}", id);
	return nullptr;
}

// ================================== (Monster) ========================================= //
MonsterType stringToMonsterType(const std::string &stateStr)
{
	if (stateStr == "Attack")
		return MonsterType::ATTACK;
	if (stateStr == "Summon")
		return MonsterType::SUMMON;
	if (stateStr == "Wander")
		return MonsterType::WANDER;
	if (stateStr == "Boss")
		return MonsterType::BOSS;
}

std::unordered_map<AttackStrategies, std::shared_ptr<IAttackStrategy>>
stringToAtkStrategies(const nlohmann::json &atkStrategies)
{
	std::unordered_map<AttackStrategies, std::shared_ptr<IAttackStrategy>> strategies;
	for (const auto &atkType : atkStrategies)
	{
		if (atkType == "Collision")
			strategies[AttackStrategies::COLLISION_ATTACK] = std::make_shared<CollisionAttack>();
		if (atkType == "Melee")
			strategies[AttackStrategies::MELEE] = std::make_shared<MeleeAttack>();
		if (atkType == "Gun")
			strategies[AttackStrategies::GUN] = std::make_shared<GunAttack>();
		if (atkType == "Boss")
			strategies[AttackStrategies::BOSS] = std::make_shared<BossAttackStrategy>();
		if (atkType == "None")
			strategies[AttackStrategies::NONE] = std::make_shared<NoAttack>();
	}
	return strategies;
}

bool JsonArrayContains(const nlohmann::json &array, const std::string &target)
{
	if (!array.is_array())
		return false;

	return std::any_of(array.begin(), array.end(), [&target](const auto &item)
					   { return item.is_string() && item.template get<std::string>() == target; });
}

std::shared_ptr<Character> CharacterFactory::createEnemy(const int id)
{
	// 在 JSON 陣列中搜尋符合名稱的角色
	for (const auto &characterInfo : enemyJsonData)
	{
		if (characterInfo["ID"] == id)
		{
			std::string name = characterInfo["name"].get<std::string>();
			CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
			std::shared_ptr<Character> enemy = std::make_shared<Character>(name, type);
			enemy->SetZIndexType(ZIndexType::OBJECTHIGH);

			// 檢查是否是精英形態
			if (characterInfo["isElite"].get<bool>() == true)
				enemy->m_Transform.scale = glm::vec2(1.4f);
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
			if (aiType == MonsterType::SUMMON)
			{
				moveStrategy = std::make_shared<NoMove>();
				utilityStrategy = std::make_shared<SummonUtility>();
			}
			else if (aiType == MonsterType::ATTACK)
			{
				moveStrategy = std::make_shared<ChaseMove>();
			}
			else if (aiType == MonsterType::WANDER)
			{
				moveStrategy = std::make_shared<WanderMove>();
			}
			else if (aiType == MonsterType::BOSS)
				moveStrategy = std::make_shared<BossMove>();
			else
				LOG_ERROR("{}'s moveType not found", id);

			// 根據攻擊類型
			int haveWeapon = characterInfo["haveWeapon"].get<int>();
			if (haveWeapon == 0)
			{
				if (isCollisionAttack)
					collisionDamage = characterInfo["collisionDamage"].get<int>();
			}
			else
			{
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
			auto attackComp =
				enemy->AddComponent<AttackComponent>(ComponentType::ATTACK, weapon, 0, 0, collisionDamage);
			auto aiComp = enemy->AddComponent<AIComponent>(ComponentType::AI, aiType, moveStrategy, attackStrategies,
														   utilityStrategy, monsterPoint);
			auto collisionComp = enemy->AddComponent<CollisionComponent>(ComponentType::COLLISION);
			collisionComp->SetCollisionLayer(CollisionLayers_Enemy);
			if (haveWeapon == 0 && isCollisionAttack)
				collisionComp->AddCollisionMask(CollisionLayers_Player);
			collisionComp->AddCollisionMask(CollisionLayers_Terrain);
			collisionComp->AddCollisionMask(CollisionLayers_DestructibleTerrain);
			collisionComp->AddCollisionMask(CollisionLayers_Player_Projectile);
			collisionComp->AddCollisionMask(CollisionLayers_Player_EffectAttack);
			collisionComp->SetSize(glm::vec2(bodySize));
			collisionComp->SetOffset(glm::vec2(0.0f, -6.0f));

			return enemy;
		}
	}

	LOG_ERROR("{}'s ID not found: {}", id);
	return nullptr;
}


// ================================== (Monster) ========================================= //
InteractableType stringToInteractableType(const std::string &stateStr)
{
	if (stateStr == "NPC_MERCHANT")
		return InteractableType::NPC_MERCHANT;
	if (stateStr == "NPC_RICH_GUY")
		return InteractableType::NPC_RICH_GUY;
	if (stateStr == "NPC_HELLO_WORLD")
		return InteractableType::NPC_HELLO_WORLD;
}

std::shared_ptr<Character> CharacterFactory::createNPC(const int id)
{
	// 在 JSON 陣列中搜尋符合名稱的角色
	for (const auto& characterInfo : npcJsonData)
	{
		if (characterInfo["ID"] == id)
		{
			std::string name = characterInfo["name"].get<std::string>();
			CharacterType type = stringToCharacterType(characterInfo["Type"].get<std::string>());
			std::shared_ptr<Character> npc = std::make_shared<Character>(name, type);
			npc->SetZIndexType(ZIndexType::OBJECTHIGH);

			auto animation = parseCharacterAnimations(characterInfo["animations"]);
			
			// 設置 InteractableComponent
			StructInteractableComponent interactable;
			interactable.s_InteractableType = stringToInteractableType(characterInfo["interactableType"].get<std::string>());
			interactable.s_InteractionRadius = characterInfo["interactionRadius"].get<float>();
			interactable.s_IsAutoInteract = characterInfo["isAutoInteract"].get<bool>();
			
			// 設置 NPCComponent
			StructNPCComponent npcConfig;
			if (characterInfo.contains("dialogues") && characterInfo["dialogues"].is_array()) {
				for (const auto& dialogue : characterInfo["dialogues"]) {
					npcConfig.dialogues.push_back(dialogue.get<std::string>());
				}
			}
			// 只有当prompt不为空时才设置
			if (!characterInfo["prompt"].get<std::string>().empty()) {
				npcConfig.promptText = characterInfo["prompt"].get<std::string>();
				npcConfig.promptSize = characterInfo["promptSize"].get<float>();
			}

			auto animationComp = npc->AddComponent<AnimationComponent>(ComponentType::ANIMATION, animation);
			auto stateComp = npc->AddComponent<StateComponent>(ComponentType::STATE);
			auto npcComp = npc->AddComponent<NPCComponent>(ComponentType::NPC, npcConfig);
			auto interactableComp = npc->AddComponent<InteractableComponent>(ComponentType::INTERACTABLE, interactable);

			// 如果是NPC_HELLO_WORLD，创建提示动画
			if (interactable.s_InteractableType == InteractableType::NPC_HELLO_WORLD) {
				if (characterInfo.contains("promptAnimation")) {
					auto promptAnim = createAnimationFromConfig(characterInfo["promptAnimation"]);
					interactableComp->SetPromptAnimation(promptAnim);
				}
			}

			return npc;
		}
	}

	LOG_ERROR("{}'s ID not found: {}", id);
	return nullptr;
}