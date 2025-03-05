//
// Created by QuzzS on 2025/3/1.
//

#ifndef NGAMEOBJECT_HPP
#define NGAMEOBJECT_HPP

#include "Util/GameObject.hpp"

class nGameObject : public Util::GameObject
{
public:
	enum class GameObjectDirection
	{
		Null,
		Forward,
		Reverse,
	};
	
	glm::vec2 m_WorldCoord = {0,0};

	[[nodiscard]] glm::vec2 GetPivot() const { return m_Pivot;}
	
	[[nodiscard]] GameObjectDirection GetImageDirection() const { return m_ImageDirection;}

	[[nodiscard]] glm::vec2 GetWorldCoord() const {return m_WorldCoord;}

	void SetHorizontalMirrorDrawable(
		const std::shared_ptr<Core::Drawable> &forwardDrawable,
		const std::shared_ptr<Core::Drawable> &reverseDrawable)
	{
		m_Drawable = forwardDrawable;
		m_ForwardDrawable = forwardDrawable;
		m_ReverseDrawable = reverseDrawable;
		m_ImageDirection = GameObjectDirection::Forward;
	}

	void ChangeDrawableDirection()
	{
		if (m_ImageDirection == GameObjectDirection::Null)
		{
			LOG_ERROR("nGameObject unable m_ImageDirection");
			return;
		}
		
		switch (m_ImageDirection)
		{
		case GameObjectDirection::Forward:
			m_Drawable = m_ReverseDrawable;
			m_ImageDirection = GameObjectDirection::Reverse;
			break;
		case GameObjectDirection::Reverse:
			m_Drawable = m_ForwardDrawable;
			m_ImageDirection = GameObjectDirection::Forward;
			break;
		default:
			break;
		}
	}

protected:
	std::shared_ptr<Core::Drawable> m_ForwardDrawable = nullptr;
	std::shared_ptr<Core::Drawable> m_ReverseDrawable = nullptr;
	GameObjectDirection m_ImageDirection = GameObjectDirection::Null;

};

#endif //NGAMEOBJECT_HPP
