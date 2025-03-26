//
// Created by QuzzS on 2025/3/1.
//

//nGameObject.hpp

#ifndef NGAMEOBJECT_HPP
#define NGAMEOBJECT_HPP

#include "Util/GameObject.hpp"
#include "Util/Logger.hpp"
#include "Components/Component.hpp"

class nGameObject : public Util::GameObject, public std::enable_shared_from_this<nGameObject> //爲了讓Component可以指向nGameObject
{
public:
	explicit nGameObject(const std::string& objectName = "")
		: m_Name(objectName.empty() ? "GameObject_" + std::to_string(GetNextId()) : objectName ) {}

	~nGameObject() override = default;

	glm::vec2 m_WorldCoord = {0,0};

	// Components
	template <typename T, typename... Args>
	std::shared_ptr<T> AddComponent(ComponentType type, Args &&...args);
	template <typename T>
	std::shared_ptr<T> GetComponent(ComponentType type);

	virtual void Update()
	{
		if (!m_Active) return;

		for (auto& [type, component] : m_Components) {
			component->Update();  // 更新每個組件
		}
	}
	void onCollision(const std::shared_ptr<nGameObject>& other, CollisionInfo& info)
	{
		LOG_DEBUG("Collision");
		if (!m_Active) return;
		for (const auto& pair : m_Components) {
			// pair.first 是 ComponentType 键
			// pair.second 是 std::shared_ptr<Component> 值
			pair.second->HandleCollision(info);
		}
	}

	void SetActive(const bool active) { m_Active = active; }
	[[nodiscard]] bool IsActive() const { return m_Active; }

	// Getter
	[[nodiscard]] std::string GetName() const { return m_Name; }
	[[nodiscard]] virtual std::string GetClassName() const { return "nGameObject"; }
	[[nodiscard]] glm::vec2 GetPivot() const { return m_Pivot;}
	[[nodiscard]] glm::vec2 GetImageSize() const {return m_Drawable->GetSize();}
	[[nodiscard]] glm::vec2 GetWorldCoord() const {return m_WorldCoord;}
	std::shared_ptr<Core::Drawable> GetDrawable() const {return m_Drawable;}

protected:
	std::string m_Name;
	bool m_Active = true;
	std::unordered_map<ComponentType, std::shared_ptr<Component>> m_Components;

private:
	static int GetNextId()
	{
		static int gameObjectCounter = 0;
		return gameObjectCounter++;
	}
};

#include "nGameObject.inl" //必須先完成定義后才能包含

#endif //NGAMEOBJECT_HPP
