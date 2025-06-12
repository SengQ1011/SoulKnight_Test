//
// Created by QuzzS on 2025/4/19.
//

#ifndef COLLISIONCOMPONENTSTRUCT_HPP
#define COLLISIONCOMPONENTSTRUCT_HPP

#include <unordered_map>
#include "EnumTypes.hpp"
#include "glm/fwd.hpp" // glm::uint8_t
#include "glm/vec2.hpp"
#include "json.hpp"

#include "EventInfo.hpp"

class nGameObject;

enum CollisionLayers : glm::uint8_t { //不用class因爲不想重載運算子直接用uint8_t的運算子
	CollisionLayers_None = 0,				//00000000
	CollisionLayers_Player = 1 << 0,		//00000001 玩家層
	CollisionLayers_Enemy = 1 << 1,			//00000010 敵人層
	CollisionLayers_Player_Projectile = 1 << 2,	//00000100 玩家子彈層
	CollisionLayers_Enemy_Projectile = 1 << 3,	//00001000 敵人子彈層
	CollisionLayers_Player_EffectAttack = 1 << 4,	//00010000 玩家子彈層
	CollisionLayers_Enemy_EffectAttack = 1 << 5,	//00100000 敵人子彈層
	CollisionLayers_DestructibleTerrain = 1 << 6,		// 拾取物層
	CollisionLayers_Terrain = 1 << 7,		// 地形層
};

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
	{"Player_Projectile", CollisionLayers_Player_Projectile},
	{"Enemy_Projectile", CollisionLayers_Enemy_Projectile},
	{"Player_EffectAttack", CollisionLayers_Player_EffectAttack},
	{"Enemy_EffectAttack", CollisionLayers_Enemy_EffectAttack},
	{"DestructibleTerrain", CollisionLayers_DestructibleTerrain},
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

// TODO: 嘗試重構中
struct CollisionEventInfo final : TypedEventInfo<CollisionEventInfo>
{
private:
	glm::vec2 collisionNormal;
	//確定碰撞方向， 反彈角度 Ex摩檫力作用方向=切綫方向
	//必須通過Set和Get來設定

public:
	std::weak_ptr<nGameObject> objectA; //避免循環引用 不要因爲這個Info而導致objectA因爲被引用而無法銷毀
	std::weak_ptr<nGameObject> objectB;
	float penetration;

	CollisionEventInfo(const std::shared_ptr<nGameObject> &objectA, const std::shared_ptr<nGameObject> &objectB) :
		TypedEventInfo(EventType::Collision),
		collisionNormal(0),
		objectA(objectA),
		objectB(objectB),
		penetration(0) {}
	~CollisionEventInfo() override = default;

	[[nodiscard]] std::shared_ptr<nGameObject> GetObjectA() const {return objectA.lock();}//lock可以暫時取得std::shared_ptr
	[[nodiscard]] std::shared_ptr<nGameObject> GetObjectB() const {return objectB.lock();}

	//標準化碰撞法綫 ->只有方向信息，不含大小
	void SetCollisionNormal (const glm::vec2& normal);
	[[nodiscard]] glm::vec2 GetCollisionNormal() const;
};
// TODO: 保佑通過

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
	glm::vec2 m_Position;	//物體中心世界坐標
	glm::vec2 m_Size;		//矩形長寬
	// 需要旋轉嗎？

	Rect(const glm::vec2& position, const glm::vec2& size)
		: m_Position(position), m_Size(size) {}

	[[nodiscard]] float left() const { return m_Position.x - m_Size.x/2.0f; }
	[[nodiscard]] float right() const { return m_Position.x + m_Size.x/2.0f; }
	[[nodiscard]] float bottom() const { return m_Position.y - m_Size.y/2.0f; }
	[[nodiscard]] float top() const { return m_Position.y + m_Size.y/2.0f; }

	// 判斷兩個矩形是否有交集
	[[nodiscard]] bool Intersects(const Rect& other) const;
};


#endif //COLLISIONCOMPONENTSTRUCT_HPP
