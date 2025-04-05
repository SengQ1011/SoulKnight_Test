//
// Created by tjx20 on 3/10/2025.
//

#ifndef CHARACTERFACTORY_HPP
#define CHARACTERFACTORY_HPP

#include "EnumTypes.hpp"
#include "Factory/Factory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "Components/AiComponent.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/SkillComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"

// 角色工廠：根據名稱創建角色
class CharacterFactory: public Factory {
public:
	static CharacterFactory& GetInstance() {
		static CharacterFactory instance;
		return instance;
	}

	// 根據角色配置文件創建角色
	std::shared_ptr<Character> createPlayer(const int id);
	std::shared_ptr<Character> createEnemy(const int id);
	std::shared_ptr<Character> CloneEnemy(const std::shared_ptr<Character>& original);
	void ClearCache();  // 缓存清理功能

private:
	static CharacterFactory* instance;
	nlohmann::json enemyJsonData;  // 缓存JSON数据（只需读取一次文件）

	// 预加载JSON
	CharacterFactory() {enemyJsonData = readJsonFile("enemy.json");}
	~CharacterFactory() = default;
	// 删除拷贝操作
	CharacterFactory(const CharacterFactory&) = delete;
	void operator=(const CharacterFactory&) = delete;
};

#endif // CHARACTERFACTORY_HPP