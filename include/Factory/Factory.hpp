//
// Created by tjx20 on 3/14/2025.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "Creature/Character.hpp"
#include "Creature/Enemy.hpp"
#include "Creature/Player.hpp"
#include "Weapon/Weapon.hpp"
#include "Components/AiComponent.hpp"
#include "Components/AnimationComponent.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/FollowerComponent.hpp"
#include "Components/HealthComponent.hpp"
#include "Components/InputComponent.hpp"
#include "Components/MovementComponent.hpp"
#include "Components/StateComponent.hpp"
#include <json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

class Factory {
public:
	Factory();
	virtual ~Factory() = default;
	// 讀取 JSON 文件
	nlohmann::json readJsonFile(const std::string& fileName);
	static std::shared_ptr<Component> createComponent(const nlohmann::json& json);
};


#endif //FACTORY_HPP
