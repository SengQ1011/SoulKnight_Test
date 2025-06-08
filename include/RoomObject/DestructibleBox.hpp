//
// Created by Assistant on 2025/1/20.
//

#ifndef DESTRUCTIBLEBOX_HPP
#define DESTRUCTIBLEBOX_HPP

#include <string>
#include "Components/HealthComponent.hpp"
#include "Override/nGameObject.hpp"


// 前向聲明
struct EventInfo;

// 可破壞木箱類別 - 排除在碰撞優化之外
class DestructibleBox : public nGameObject
{
public:
	explicit DestructibleBox(const std::string &baseName = "destructible_box");

	~DestructibleBox() override = default;

	// 重寫類別名稱
	std::string GetClassName() const override { return "DestructibleBox"; }

	// 重寫事件接收處理
	void OnEventReceived(const EventInfo &eventInfo) override;

	// 破壞相關方法
	void OnDestroyed();
	bool IsDestroyed() const { return m_IsDestroyed; }

private:
	bool m_IsDestroyed = false;
};

#endif // DESTRUCTIBLEBOX_HPP
