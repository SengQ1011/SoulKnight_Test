//
// Created by QuzzS on 2025/3/15.
//

// CollisionComponent.hpp

#ifndef COLLISIONCOMPONENT_HPP
#define COLLISIONCOMPONENT_HPP

#include "Component.hpp"
#include "Structs/CollisionComponentStruct.hpp"
#include "TriggerStrategy/ITriggerStrategy.hpp"

#include <unordered_set>
#include "Override/nGameObject.hpp"
#include "Structs/AreaShape.hpp"


enum class ShapeType;
class AreaShape;
namespace Util
{
	class Image;
}

// TODO: SAT旋轉矩形還沒實現，在GetAreaShape裏面沒有Rect更改碰撞箱Rotate
class CollisionComponent final : public Component
{
public:
	explicit CollisionComponent(const ComponentType type = ComponentType::COLLISION,
								const glm::vec2 &size = glm::vec2(0), const glm::vec2 &offset = glm::vec2(0),
								const glm::uint8_t collisionLayer = CollisionLayers_None,
								const glm::uint8_t collisionMask = CollisionLayers_None, const bool isTrigger = false) :
		Component(type), m_Size(size), m_Offset(offset), m_CollisionLayer(collisionLayer),
		m_CollisionMask(collisionMask), m_IsTrigger(isTrigger)
	{
	}

	explicit CollisionComponent(const StructCollisionComponent &data) :
		Component(data.m_Type), m_Size(glm::vec2(data.m_Size[0], data.m_Size[1])),
		m_Offset(data.m_Offset[0], data.m_Offset[1]), m_CollisionLayer(data.m_CollisionLayer),
		m_CollisionMask(data.m_CollisionMask), m_IsTrigger(data.m_IsTrigger),
		m_TriggerStrategyType(data.m_TriggerStrategyType), m_TriggerStrategyConfig(data.m_TriggerStrategyConfig)
	{
	}

	~CollisionComponent() override = default;

	void Init() override;
	void Update() override;

	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

	void TryTrigger(const std::shared_ptr<nGameObject> &self, const std::shared_ptr<nGameObject> &other);
	void FinishTriggerFrame(const std::shared_ptr<nGameObject> &self);

	[[nodiscard]] bool CanCollideWith(const std::shared_ptr<CollisionComponent> &other) const;

	// Getter
	[[nodiscard]] Rect GetBounds() const;
	[[nodiscard]] glm::vec2 GetSize() const { return m_Size; };
	[[nodiscard]] std::shared_ptr<AreaShape> GetAreaShape() const; // 新添加
	[[nodiscard]] glm::uint8_t GetCollisionLayer() const { return m_CollisionLayer; }
	[[nodiscard]] glm::uint8_t GetCollisionMask() const { return m_CollisionMask; }
	[[nodiscard]] bool IsActive() const { return m_IsActive; }
	[[nodiscard]] bool IsTrigger() const { return m_IsTrigger; }
	[[nodiscard]] bool IsCollider() const { return m_IsCollider; }
	std::shared_ptr<AreaShape> GetTriggerAreaShape() const;
	std::shared_ptr<AreaShape> GetColliderAreaShape() const;

	// Setter
	void SetCollisionLayer(const glm::uint8_t collisionLayer) { m_CollisionLayer = collisionLayer; }
	void AddCollisionLayer(const glm::uint8_t collisionLayer) { m_CollisionLayer |= collisionLayer; }
	void AddCollisionMask(const glm::uint8_t collisionMask) { m_CollisionMask |= collisionMask; }
	void ResetCollisionMask() { m_CollisionMask = CollisionLayers_None; }
	void SetOffset(const glm::vec2 &offset) { m_Offset = offset; }
	void SetSize(const glm::vec2 &size) { m_Size = size; }
	void SetActive(const bool isActive) { m_IsActive = isActive; }
	void SetTrigger(const bool isTrigger) { m_IsTrigger = isTrigger; }
	void AddTriggerStrategy(std::unique_ptr<ITriggerStrategy> strategy);
	void ClearTriggerStrategies();
	void ClearTriggerTargets();
	void SetCollider(const bool isCollider) { m_IsCollider = isCollider; }

private:
	bool m_IsActive = true;

	// 强大的扳機 可以殺光一切
	bool m_IsTrigger = false;
	ShapeType m_TriggerShapeType = ShapeType::Null; // 當Trigger沒有範圍卻又IsTrigger==True,自動跟隨Collider範圍
	std::shared_ptr<AreaShape> m_TriggerAreaShape = nullptr;
	std::vector<std::unique_ptr<ITriggerStrategy>> m_TriggerStrategies;
	std::unordered_set<std::shared_ptr<nGameObject>>
		m_PreviousTriggerTargets; // 本幀（frame）裡所有經由 TryTrigger(self, other) 檢測到與這個 Trigger
								  // 物件發生交集的其他物件集合
	std::unordered_set<std::shared_ptr<nGameObject>>
		m_CurrentTriggerTargets; // （上一幀）結束時，紀錄下那些「還在觸發」的物件集合

	bool m_IsCollider = true;
	ShapeType m_ColliderShapeType = ShapeType::Null;
	std::shared_ptr<AreaShape> m_ColliderAreaShape = nullptr;

	// 原本的先不動
	glm::vec2 m_Size;
	glm::vec2 m_Offset;
	glm::uint8_t m_CollisionLayer; // 自身碰撞層
	glm::uint8_t m_CollisionMask; // 可以和哪幾層碰撞

	std::string m_TriggerStrategyType;
	nlohmann::json m_TriggerStrategyConfig;
};


#endif // COLLISIONCOMPONENT_HPP
