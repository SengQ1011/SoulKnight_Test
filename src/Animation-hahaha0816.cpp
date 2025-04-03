//
// Created by tjx20 on 3/14/2025.
//

#include "Animation.hpp"
#include "Util/Logger.hpp"

Animation::Animation(std::vector<std::string> AnimationPaths)
	: m_AnimationPaths(std::move(AnimationPaths)) {
	m_Drawable = std::make_shared<Util::Animation>(m_AnimationPaths, false, 500, false, 0);
	for (const auto& AnimationPath : m_AnimationPaths)
	{
		LOG_DEBUG("AnimationPath: {}", AnimationPath);
	}
	this->SetZIndex(12);
	SetLooping(true);
}

bool Animation::IfAnimationEnds() const {
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	return animation && animation->GetCurrentFrameIndex() == animation->GetFrameCount() - 1;
}

// 自动播放（内部控制更新频率）
void Animation::PlayAnimation(bool play) {
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	animation->SetInterval(1000.0f / 30.0f);
	if (animation && animation->GetState() != Util::Animation::State::PLAY && play) {
		animation->Play();  // 開始播放動畫
	} else {
		animation->Pause();
	}
}

