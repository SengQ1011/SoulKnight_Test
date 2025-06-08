//
// Created by Assistant on 2025/1/20.
//

#ifndef DESTRUCTIBLEOBJECT_HPP
#define DESTRUCTIBLEOBJECT_HPP

#include <string>
#include "Components/HealthComponent.hpp"
#include "Override/nGameObject.hpp"


// 前向聲明
struct EventInfo;

// 可破壞物件類別 - 排除在碰撞優化之外
class DestructibleObject : public nGameObject
{
public:
	explicit DestructibleObject(const std::string &baseName = "destructible_object");

	~DestructibleObject() override = default;

	// 重寫類別名稱
	std::string GetClassName() const override { return "DestructibleObject"; }

	// 重寫事件接收處理
	void OnEventReceived(const EventInfo &eventInfo) override;

	// 破壞相關方法
	void OnDestroyed();
	bool IsDestroyed() const { return m_IsDestroyed; }

private:
	bool m_IsDestroyed = false;
};

#endif // DESTRUCTIBLEOBJECT_HPP
