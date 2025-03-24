//
// Created by tjx20 on 3/10/2025.
//

#ifndef CHARACTERFACTORY_HPP
#define CHARACTERFACTORY_HPP

#include "Factory/Factory.hpp"
#include "Factory/WeaponFactory.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/StateComponent.hpp"
#include "Tool/CollisionBox.hpp"

// 角色工廠：根據名稱創建角色
class CharacterFactory: public Factory {
public:
	CharacterFactory();
	~CharacterFactory() override = default;
	// 根據角色配置文件創建角色
	std::shared_ptr<Player> createPlayer(const int id);
	std::shared_ptr<Enemy> createEnemy(const int id);

private:
	WeaponFactory wf;
};

#endif // CHARACTERFACTORY_HPP