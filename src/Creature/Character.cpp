//
// Created by QuzzS on 2025/3/4.
//

#include "Creature/Character.hpp"
#include "ObserveManager/AudioManager.hpp"
#include "Structs/EventInfo.hpp"

Character::Character(const std::string &name, CharacterType type) : m_name(name), m_type(type)
{
	ResetPosition();
	this->SetPivot(glm::vec2{0.5f, 0.5f});
}

void Character::OnShowUp()
{
	// 顯示角色
	SetControlVisible(true);

	// 播放角色出現音效
	AudioManager::GetInstance().PlaySFX("show_up");
}

void Character::OnHide()
{
	// 隱藏角色
	SetControlVisible(false);
}

void Character::OnEventReceived(const EventInfo &eventInfo)
{
	// 監聽ShowUp和Hide事件
	if (eventInfo.GetEventType() == EventType::ShowUp)
	{
		OnShowUp();
	}
	else if (eventInfo.GetEventType() == EventType::Hide)
	{
		OnHide();
	}
}
