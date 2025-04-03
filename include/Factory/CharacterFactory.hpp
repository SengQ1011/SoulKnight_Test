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
#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"

// 角色工廠：根據名稱創建角色
class CharacterFactory: public Factory {
public:
	static CharacterFactory& GetInstance() {
		if (!instance) instance = new CharacterFactory();
		return *instance;
	}

	// 删除拷贝操作
	CharacterFactory(const CharacterFactory&) = delete;
	void operator=(const CharacterFactory&) = delete;

	// 根據角色配置文件創建角色
	std::shared_ptr<Character> createPlayer(const int id);
	std::shared_ptr<Character> createEnemy(const int id);

private:
	static CharacterFactory* instance;
	CharacterFactory() = default;
	std::unordered_map<int, std::shared_ptr<Character>> enemyTemplates;		// 緩存
};

#endif // CHARACTERFACTORY_HPP