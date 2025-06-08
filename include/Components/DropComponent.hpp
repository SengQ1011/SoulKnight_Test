//
// Created by Assistant on 2025/1/20.
//

#ifndef DROPCOMPONENT_HPP
#define DROPCOMPONENT_HPP

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include "Components/Component.hpp"

class nGameObject;

/**
 * @brief 掉落組件
 * 負責監聽物體破壞事件並生成掉落物品（如金幣、能量球等）
 */
class DropComponent : public Component
{
public:
	enum class ScatterMode
	{
		RANDOM, // 隨機散佈（默認）
		FIXED, // 固定在中心位置
		SLIGHT // 輕微散佈
	};

	explicit DropComponent(ScatterMode mode = ScatterMode::RANDOM, float radius = 30.0f);
	~DropComponent() override = default;

	void Init() override;
	void HandleEvent(const EventInfo &eventInfo) override;
	std::vector<EventType> SubscribedEventTypes() const override;

	/**
	 * @brief 觸發掉落效果
	 * @param position 掉落位置
	 */
	void TriggerDrop(const glm::vec2 &position);

	/**
	 * @brief 添加掉落物品（預先創建好的物件）
	 * @param dropItems 掉落物品列表
	 */
	void AddDropItems(const std::vector<std::shared_ptr<nGameObject>> &dropItems);

private:
	std::vector<std::shared_ptr<nGameObject>> m_dropItems; // 掉落物品列表
	bool m_hasDropped = false; // 是否已掉落，防止重複掉落
	ScatterMode m_scatterMode = ScatterMode::RANDOM; // 散佈模式
	float m_scatterRadius = 30.0f; // 散佈半徑
};

#endif // DROPCOMPONENT_HPP
