//
// Created by tjx20 on 3/14/2025.
//

#include "Animation.hpp"
#include "Util/Logger.hpp"

Animation::Animation(const std::vector<std::string> &AnimationPaths, bool needLoop, const std::string &_class,
					 float interval) :
	m_AnimationPaths(AnimationPaths), m_Looping(needLoop), nGameObject("Animation", _class)
{
	if (interval == 0)
	{
		if (const int frameCount = m_AnimationPaths.size(); frameCount > 0)
		{
			if (frameCount <= 2)
			{
				interval = 250.0f; // 4FPS
			}
			else if (frameCount <= 5)
			{
				interval = 125.0f; // 8FPS
			}
			else
			{
				interval = 1000.0f / (frameCount + 10);
				interval = std::clamp(interval, 50.0f, 100.0f);
			}
		}
		else
		{
			LOG_ERROR("Animation::PlayAnimation: AnimationPaths is empty");
		}
	}
	m_Drawable = std::make_shared<Util::Animation>(m_AnimationPaths, false, interval, m_Looping, 0);
	this->SetZIndexType(ZIndexType::CUSTOM);
}

bool Animation::IfAnimationEnds() const
{
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	return animation && !animation->GetLooping() && // 確保不是循環動畫
		animation->GetState() == Util::Animation::State::ENDED; // 播放狀態為 ENDED（代表播放完畢）
}

// 自动播放（内部控制更新频率）
void Animation::PlayAnimation(bool play)
{
	const auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	if (animation && animation->GetState() != Util::Animation::State::PLAY && play)
	{
		animation->SetCurrentFrame(0); // 重置到第一帧
		animation->Play(); // 開始播放動畫
	}
	else
	{
		animation->Pause();
	}
}
