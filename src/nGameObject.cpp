//
// Created by QuzzS on 2025/4/27.
//

#include "Override/nGameObject.hpp"

void nGameObject::Update()
{
	if (!m_Active) return;

	for (auto& [type, component] : m_Components) {
		component->Update();  // 更新每個組件
	}
}

std::string nGameObject::GenerateUniqueName(const std::string &baseName)
{
	static std::unordered_map<std::string, int> nameCounter;
	if (baseName.empty())
	{
		return "GameObject_" + std::to_string(nameCounter["GameObject"]++);
	}
	const int id = nameCounter[baseName]++;
	return baseName + std::to_string(id);
}

int nGameObject::GenerateUniqueID() {
	static int currentId = 0;
	return currentId++;
}
