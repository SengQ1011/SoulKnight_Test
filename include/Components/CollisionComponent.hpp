//
// Created by QuzzS on 2025/3/15.
//

//CollisionComponent.hpp

#ifndef COLLISIONCOMPONENT_HPP
#define COLLISIONCOMPONENT_HPP

#include "pch.hpp"

#include "Component.hpp"
#include "json.hpp"
#include "Override/nGameObject.hpp"

enum CollisionLayers : glm::uint8_t { //不用class因爲不想重載運算子直接用uint8_t的運算子
	CollisionLayers_None = 0,				//00000000
	CollisionLayers_Player = 1 << 0,		//00000001 玩家層
	CollisionLayers_Enemy = 1 << 1,			//00000010 敵人層
	CollisionLayers_Player_Bullet = 1 << 2,	//00000100 玩家子彈層
	CollisionLayers_Enemy_Bullet = 1 << 3,	//00001000 玩家子彈層
	CollisionLayers_Pickup = 1 << 4,		//00010000 拾取物層
	CollisionLayers_Terrain = 1 << 5,		//00100000 地形層
};

namespace StructComponents {
	struct StructCollisionComponent
	{
		ComponentType m_Type = ComponentType::COLLISION;
		std::string m_Class = "collisionComponent";
		std::array<float, 2> m_Size{};
		std::array<float, 2> m_Offset{}; //左下角位置 = Object原點 + m_Offset
		uint8_t m_CollisionLayer = CollisionLayers_None;
		uint8_t m_CollisionMask = CollisionLayers_None;
		bool m_IsTrigger = false;
	};

	inline std::unordered_map<std::string, CollisionLayers> m_String2CollisionLayer = {
		{"None", CollisionLayers_None},
		{"Player", CollisionLayers_Player},
		{"Enemy", CollisionLayers_Enemy},
		{"Player_Bullet", CollisionLayers_Player_Bullet},
		{"Enemy_Bullet", CollisionLayers_Enemy_Bullet},
		{"Pickup", CollisionLayers_Pickup},
		{"Terrain", CollisionLayers_Terrain},
	};

	inline void to_json(nlohmann::ordered_json &j, const StructCollisionComponent &c) {
		j = nlohmann::ordered_json{
	          {"Type", c.m_Type},
			  {"Class", c.m_Class},
			  {"Size", c.m_Size},
			  {"Offset",c.m_Offset},
			  {"CollisionLayer",c.m_CollisionLayer},
			  {"CollisionMask",c.m_CollisionMask},
			  {"IsTrigger",c.m_IsTrigger}};
	}

	inline void from_json(const nlohmann::ordered_json& j, StructCollisionComponent &c) {
		j.at("Class").get_to(c.m_Class);
		j.at("Size").get_to(c.m_Size);
		j.at("Offset").get_to(c.m_Offset);

		c.m_CollisionLayer = m_String2CollisionLayer.find(j.at("CollisionLayer").get<std::string>())->second;

		for (const auto& it : j.at("CollisionMask"))
		{
			c.m_CollisionMask |= (m_String2CollisionLayer.find(it.get<std::string>())->second);
		}
		j.at("IsTrigger").get_to(c.m_IsTrigger);
	}
}

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

class CollisionComponent final : public Component {
public:

	explicit CollisionComponent(const ComponentType type = ComponentType::COLLISION, const glm::vec2& size = glm::vec2(0),
					   const glm::vec2& offset = glm::vec2(0),
					   const glm::uint8_t collisionLayer = CollisionLayers_None,
					   const glm::uint8_t collisionMask = CollisionLayers_None,
					   const bool isTrigger = false)
		: Component(type), m_Size(size), m_Offset(offset), m_CollisionLayer(collisionLayer),
		  m_CollisionMask(collisionMask), m_IsTrigger(isTrigger)
	{

	}

	explicit CollisionComponent(const StructComponents::StructCollisionComponent& data)
		: Component(data.m_Type), m_Size(glm::vec2(data.m_Size[0], data.m_Size[1])),
		  m_Offset(data.m_Offset[0], data.m_Offset[1]), m_CollisionLayer(data.m_CollisionLayer),
		  m_CollisionMask(data.m_CollisionMask), m_IsTrigger(data.m_IsTrigger) {}

	~CollisionComponent() override = default;

	void Init() override;
	void Update() override;

	[[nodiscard]] bool CanCollideWith( const std::shared_ptr<CollisionComponent>& other ) const;

	// Getter
	[[nodiscard]] Rect GetBounds() const;
	[[nodiscard]] glm::uint8_t GetCollisionLayer() const {return m_CollisionLayer;}
	[[nodiscard]] glm::uint8_t GetCollisionMask() const {return m_CollisionMask;}
	[[nodiscard]] std::shared_ptr<nGameObject> GetBlackBox() {return m_Object;}

	// Setter
	void SetCollisionLayer(const glm::uint8_t collisionLayer) { m_CollisionLayer = collisionLayer; }
	void SetCollisionMask(const glm::uint8_t collisionMask) { m_CollisionMask = collisionMask; }
	void SetOffset(const glm::vec2& offset) { m_Offset = offset; }
	void SetSize(const glm::vec2& size) { m_Size = size; }
	void SetTrigger(const bool isTrigger) {m_IsTrigger = isTrigger;}
	void SetVisible(const bool isVisible)
	{
		m_Object->SetVisible(isVisible);
	}
	[[nodiscard]] bool IsTrigger() const {return m_IsTrigger;}

private:
	glm::vec2 m_Size;
	glm::vec2 m_Offset;
	glm::uint8_t m_CollisionLayer; //自身碰撞層
	glm::uint8_t m_CollisionMask;  //可以和哪幾層碰撞
	bool m_IsTrigger;
	std::shared_ptr<nGameObject> m_Object = std::make_shared<nGameObject>();
};

#endif //COLLISIONCOMPONENT_HPP
