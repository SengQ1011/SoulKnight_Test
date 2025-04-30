

//
// Created by QuzzS on 2025/3/15.
//

#ifndef COMPONENT_HPP
#define COMPONENT_HPP

#include "EnumTypes.hpp"
#include "Structs/CollisionComponentStruct.hpp"
#include "Util/Logger.hpp"

class nGameObject;

class Component
{
public:
	Component() = default;
	explicit Component(const ComponentType type) : m_type(type) {}
	virtual ~Component() = default;

	virtual void Init() {} // nGameObject在AddComponent就會自動執行 -- nGameObject.inl
	virtual void Update() {} // n
	virtual void HandleCollision(CollisionInfo &info) {}

	// template <typename EventT,typename... Args>
	virtual void HandleEvent(const EventInfo& eventInfo) { LOG_DEBUG("HandleEvent");}

	// template<typename T>
	// void HandleEventAs(const EventInfo& eventInfo) {
	// 	const auto& typedEvent = static_cast<const T&>(eventInfo);
	// 	HandleTypedEvent(typedEvent);
	// }
	//
	// // 子類覆寫這個
	// virtual void HandleTypedEvent(const CollisionEventInfo& eventInfo) { LOG_DEBUG("HandleCollisionEvent"); }

	virtual std::vector<EventType> SubscribedEventTypes() const {return {};}

	void SetOwner(const std::shared_ptr<nGameObject> &owner)
	{
		m_Owner = owner;
	} // nGameObject在AddComponent就會自動執行 -- nGameObject.inl
	template <typename T>
	std::shared_ptr<T> GetOwner() const
	{
		return std::dynamic_pointer_cast<T>(m_Owner.lock());
	}

	[[nodiscard]] ComponentType GetType() const { return m_type; }

private:
	std::weak_ptr<nGameObject> m_Owner; // 打破循環引用,只能用GetOwner取得std::shared_ptr
	ComponentType m_type; // 區別Component 比如hitbox和collision_box 方便閲讀
};


#endif //COMPONENT_HPP
