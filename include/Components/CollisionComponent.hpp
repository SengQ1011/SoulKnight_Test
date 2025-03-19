//
// Created by QuzzS on 2025/3/15.
//

//CollisionComponent.hpp

#ifndef COLLISIONCOMPONENT_HPP
#define COLLISIONCOMPONENT_HPP

#include "pch.hpp"

#include "Component.hpp"
#include "Override/nGameObject.hpp"

struct CollisionInfo
{
private:
	glm::vec2 collisionNormal;
	//確定碰撞方向， 反彈角度 Ex摩檫力作用方向=切綫方向
	//必須通過Set和Get來設定

public:
	std::weak_ptr<nGameObject> objectA; //避免循環引用 不要因爲這個Info而導致objectA因爲被引用而無法銷毀
	std::weak_ptr<nGameObject> objectB;
	float penetration;

	CollisionInfo(const std::shared_ptr<nGameObject>& objectA, const std::shared_ptr<nGameObject>& objectB)
		: collisionNormal(0), objectA(objectA), objectB(objectB), penetration(0) {}

	[[nodiscard]] std::shared_ptr<nGameObject> GetObjectA() const {return objectA.lock();}//lock可以暫時取得std::shared_ptr
	[[nodiscard]] std::shared_ptr<nGameObject> GetObjectB() const {return objectB.lock();}

	//標準化碰撞法綫 ->只有方向信息，不含大小
	void SetCollisionNormal (const glm::vec2& normal);
	[[nodiscard]] glm::vec2 GetCollisionNormal() const;
};

struct Rect //AABB的矩形結構
{
	glm::vec2 m_Position;	//左下角世界坐標
	glm::vec2 m_Size;		//矩形長寬
	// 需要旋轉嗎？

	Rect(const glm::vec2& position, const glm::vec2& size)
		: m_Position(position), m_Size(size) {}

	[[nodiscard]] float left() const { return m_Position.x; }
	[[nodiscard]] float right() const { return m_Position.x + m_Size.x; }
	[[nodiscard]] float bottom() const { return m_Position.y; }
	[[nodiscard]] float top() const { return m_Position.y+ m_Size.y; }

	// 判斷兩個矩形是否有交集
	[[nodiscard]] bool Intersects(const Rect& other) const;
};

enum CollisionLayers : glm::uint8_t { //不用class因爲不想重載運算子直接用uint8_t的運算子
	CollisionLayers_None = 0,				//00000000
	CollisionLayers_Player = 1 << 0,		//00000001 玩家層
	CollisionLayers_Enemy = 1 << 1,			//00000010 敵人層
	CollisionLayers_Player_Bullet = 1 << 2,	//00000100 玩家子彈層
	CollisionLayers_Enemy_Bullet = 1 << 3,	//00001000 玩家子彈層
	CollisionLayers_Pickup = 1 << 4,		//00010000 拾取物層
	CollisionLayers_Terrain = 1 << 5,		//00100000 地形層
};

class CollisionComponent final : public Component {
public:

	explicit CollisionComponent(const glm::vec2& size = glm::vec2(0),
					   const glm::vec2& offset = glm::vec2(0),
					   const glm::uint8_t collisionLayer = CollisionLayers_None,
					   const glm::uint8_t collisionMask = CollisionLayers_None,
					   const bool isTrigger = false)
		: m_Size(size), m_Offset(offset), m_CollisionLayer(collisionLayer),
		  m_CollisionMask(collisionMask), m_IsTrigger(isTrigger) {}
	~CollisionComponent() override = default;

	void Init() override;
	void Update() override;

	[[nodiscard]] bool CanCollideWith( const std::shared_ptr<CollisionComponent>& other ) const;

	// Getter
	[[nodiscard]] Rect GetBounds() const;
	[[nodiscard]] glm::uint8_t GetCollisionLayer() const {return m_CollisionLayer;}
	[[nodiscard]] glm::uint8_t GetCollisionMask() const {return m_CollisionMask;}

	// Setter
	void SetCollisionLayer(const glm::uint8_t collisionLayer) { m_CollisionLayer = collisionLayer; }
	void SetCollisionMask(const glm::uint8_t collisionMask) { m_CollisionMask = collisionMask; }
	void SetTrigger(const bool isTrigger) {m_IsTrigger = isTrigger;}
	[[nodiscard]] bool IsTrigger() const {return m_IsTrigger;}

private:
	glm::vec2 m_Size;
	glm::vec2 m_Offset;
	glm::uint8_t m_CollisionLayer; //自身碰撞層
	glm::uint8_t m_CollisionMask;  //可以和哪幾層碰撞
	bool m_IsTrigger;
};

#endif //COLLISIONCOMPONENT_HPP
