//
// Created by tjx20 on 3/14/2025.
//

#ifndef FACTORY_HPP
#define FACTORY_HPP

#include "Creature/Character.hpp"
#include "Components/CollisionComponent.hpp"
#include "Weapon/Weapon.hpp"
#include <json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>

class Factory {
public:
	// 禁止複製
	Factory(Factory const&) = delete;
	void operator = (const Factory&) = delete;

	// 讀取 JSON 文件
	static nlohmann::json readJsonFile(const std::string& fileName);
	static std::shared_ptr<Component> createComponent(const nlohmann::json& json);

protected:
	static ZIndexType stringToZIndexType(const std::string& zIndexStr); // 只給子類工廠用

	// 開放建構函式給派生類，但不暴露給外部
	Factory() = default;
	virtual ~Factory() = default;
};


#endif //FACTORY_HPP
