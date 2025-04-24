//
// Created by QuzzS on 2025/3/15.
//

//CollisionComponent.hpp

#ifndef COLLISIONCOMPONENT_HPP
#define COLLISIONCOMPONENT_HPP

#include "Component.hpp"
#include "Structs/CollisionComponentStruct.hpp"

#include "Override/nGameObject.hpp" // TODO: refactor

class CollisionComponent final : public Component
{
public:
	explicit CollisionComponent(const ComponentType type = ComponentType::COLLISION,
								const glm::vec2 &size = glm::vec2(0), const glm::vec2 &offset = glm::vec2(0),
								const glm::uint8_t collisionLayer = CollisionLayers_None,
								const glm::uint8_t collisionMask = CollisionLayers_None,
								const bool isTrigger = false) :
		Component(type), m_Size(size), m_Offset(offset), m_CollisionLayer(collisionLayer),
		m_CollisionMask(collisionMask), m_IsTrigger(isTrigger)
	{
	}

	explicit CollisionComponent(const StructCollisionComponent &data) :
		Component(data.m_Type), m_Size(glm::vec2(data.m_Size[0], data.m_Size[1])),
		m_Offset(data.m_Offset[0], data.m_Offset[1]), m_CollisionLayer(data.m_CollisionLayer),
		m_CollisionMask(data.m_CollisionMask), m_IsTrigger(data.m_IsTrigger)
	{
	}

	~CollisionComponent() override = default;

	void Init() override;
	void Update() override;
	void HandleCollision(CollisionInfo &info) override;

	[[nodiscard]] bool CanCollideWith(const std::shared_ptr<CollisionComponent> &other) const;

	// Getter
	[[nodiscard]] Rect GetBounds() const;
	[[nodiscard]] glm::uint8_t GetCollisionLayer() const { return m_CollisionLayer; }
	[[nodiscard]] glm::uint8_t GetCollisionMask() const { return m_CollisionMask; }
	[[nodiscard]] std::shared_ptr<nGameObject> GetVisibleBox() { return m_ColliderVisibleBox; }
	[[nodiscard]] bool IsActive() const { return m_IsActive; }
	[[nodiscard]] bool IsTrigger() const { return m_IsTrigger; }

	// Setter
	void SetCollisionLayer(const glm::uint8_t collisionLayer) { m_CollisionLayer = collisionLayer; }
	void SetCollisionMask(const glm::uint8_t collisionMask) { m_CollisionMask |= collisionMask; }
	void ResetCollisionMask() { m_CollisionMask = CollisionLayers_None; }
	void SetOffset(const glm::vec2 &offset) { m_Offset = offset; }
	void SetSize(const glm::vec2 &size) { m_Size = size; }
	void SetActive(const bool isActive) { m_IsActive = isActive; }
	void SetTrigger(const bool isTrigger) { m_IsTrigger = isTrigger; }

	void SetColliderBoxVisible(const bool isVisible) const
	{
		m_ColliderVisibleBox->SetVisible(isVisible);
	} //設置碰撞箱可視化
	void SetColliderBoxColor(const std::string &color) const; //設置碰撞箱顔色

private:
	bool m_IsActive = true;
	glm::vec2 m_Size;
	glm::vec2 m_Offset;
	glm::uint8_t m_CollisionLayer; //自身碰撞層
	glm::uint8_t m_CollisionMask; //可以和哪幾層碰撞
	bool m_IsTrigger;
	std::shared_ptr<nGameObject> m_ColliderVisibleBox = std::make_shared<nGameObject>();
};


#endif //COLLISIONCOMPONENT_HPP
