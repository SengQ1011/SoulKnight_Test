

//
// Created by QuzzS on 2025/3/15.
//

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

//Component.hpp
#include <utility>
#include <string>
#include <memory>
#include "Util/Logger.hpp"
#include "Util/Time.hpp"
#include "InputManager.hpp"
#include "EnumTypes.hpp"

struct CollisionInfo;
class nGameObject;

class Component {
public:
	Component() = default;
	explicit Component(ComponentType type): m_type(std::move(type)) {}
	virtual ~Component() = default;

	virtual void Init() {} //nGameObject在AddComponent就會自動執行 -- nGameObject.inl
	virtual void Update() {} //n
	virtual void HandleCollision(CollisionInfo& info) {}

	void SetOwner(const std::shared_ptr<nGameObject> &owner) { m_Owner = owner; } //nGameObject在AddComponent就會自動執行 -- nGameObject.inl
	template <typename T>
	std::shared_ptr<T> GetOwner() const { return std::dynamic_pointer_cast<T>(m_Owner.lock());}

	[[nodiscard]] ComponentType GetType() const { return m_type;}

private:
	std::weak_ptr<nGameObject> m_Owner; // 打破循環引用,只能用GetOwner取得std::shared_ptr
	ComponentType m_type; // 區別Component 比如hitbox和collision_box 方便閲讀
};

#endif //COMPONENT_HPP
