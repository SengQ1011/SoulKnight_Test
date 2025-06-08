//
// Created by Assistant on 2025/1/20.
//

#ifndef DESTRUCTIBLEBOX_HPP
#define DESTRUCTIBLEBOX_HPP

#include <glm/vec2.hpp>
#include "Override/nGameObject.hpp"

// 可破壞木箱類別 - 排除在碰撞優化之外
class DestructibleBox : public nGameObject
{
public:
	explicit DestructibleBox(const std::string &baseName = "destructible_box") :
		nGameObject(baseName, "DestructibleBox") {}

	~DestructibleBox() override = default;

	// 標識這是可破壞物件
	bool IsDestructible() const { return true; }

	// 重寫類別名稱
	std::string GetClassName() const override { return "DestructibleBox"; }

	// 破壞相關方法
	void OnDestroyed();
	bool IsDestroyed() const { return m_IsDestroyed; }

private:
	bool m_IsDestroyed = false;
};

#endif // DESTRUCTIBLEBOX_HPP
