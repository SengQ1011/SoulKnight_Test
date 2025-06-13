//
// Created by Assistant on 2025/1/20.
//

#include "RoomObject/DestructibleObject.hpp"
#include "ImagePoolManager.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Structs/EventInfo.hpp"
#include "Util/Logger.hpp"


DestructibleObject::DestructibleObject(const std::string &baseName, std::vector<std::string> imagePaths) :
	nGameObject(baseName, "DestructibleObject"), m_imagePaths(imagePaths)
{
	// 初始化照片陣列
	if (!m_imagePaths.empty())
	{
		auto &imagePoolManager = ImagePoolManager::GetInstance();
		for (auto &imagePath : m_imagePaths)
		{
			const auto drawable = imagePoolManager.GetImage(RESOURCE_DIR + imagePath);
			m_drawables.emplace_back(drawable);
		}

		// 設置預設照片 (index 0)
		SetActive(true);
		SetControlVisible(true);
		SetDrawable(m_drawables[0]);
	}
}


void DestructibleObject::OnEventReceived(const EventInfo &eventInfo)
{
	// 處理破壞事件
	if (eventInfo.GetEventType() == EventType::BoxBreak)
	{
		LOG_INFO("EVENT RECEIVED BoxBreak");
		OnDestroyed();
	}
}

void DestructibleObject::OnDestroyed()
{
	if (m_IsDestroyed)
		return;

	m_IsDestroyed = true;

	LOG_DEBUG("DestructibleObject destroyed: {}", GetName());

	// 切換到破壞後的照片 (index 1)
	if (m_drawables.size() > 1)
	{
		SetDrawable(m_drawables[1]);
		SetActive(false);
		LOG_DEBUG("Switched to destroyed image for: {}", GetName());
	}
	else
	{
		// 如果沒有破壞後的照片，則隱藏物件
		SetActive(false);
		SetControlVisible(false);
		LOG_DEBUG("No destroyed image available, hiding object: {}", GetName());
	}

	if (GetName().find("box") != std::string::npos)
	{
		AudioManager::GetInstance().PlaySFX("box_destroy");
	}

	// 可以在這裡添加其他破壞效果：
	// - 播放破壞動畫
	// - 產生掉落物品
	// - 播放破壞音效
	// - 移除碰撞體
}
