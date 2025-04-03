//
// Created by QuzzS on 2025/3/15.
//

// nGameObject.inl

#ifndef NGAMEOBJECT_INL
#define NGAMEOBJECT_INL

#include "nGameObject.hpp"

template <typename T, typename... Args>
std::shared_ptr<T> nGameObject::AddComponent(ComponentType type,Args &&...args) //或許weak_ptr
{
	//確保T是Component子類別的安全判斷，如果不用樣板直接用Component，那每次使用Function就要手動ObjectCast了
	static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");
	auto component = std::make_shared<T>(std::forward<Args>(args)...);
	component->SetOwner(shared_from_this());
	m_Components[type] = component;
	component->Init();
	return component;
}

template <typename T>
std::shared_ptr<T> nGameObject::GetComponent(const ComponentType type) //或許weak_ptr
{
	// unordermap搜索法：unordered_map.find()
	auto it = m_Components.find(type);
	if (it != m_Components.end()) {
		return std::dynamic_pointer_cast<T>(it->second);
	}
	if (it == m_Components.end()) {
		LOG_WARN("Component type not found: {}", static_cast<int>(type));
		for (const auto& [key, val] : m_Components) {
			LOG_ERROR("Existing component type: {}", static_cast<int>(key));
		}
	}

	return nullptr;
}


#endif //NGAMEOBJECT_INL
