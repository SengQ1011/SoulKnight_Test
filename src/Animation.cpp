//
// Created by tjx20 on 3/14/2025.
//

#include "Animation.hpp"
#include "Util/Logger.hpp"

Animation::Animation(std::vector<std::string> AnimationPaths)
	: m_AnimationPaths(std::move(AnimationPaths)),m_CurrentFrame(0), m_ElapsedTime(0.0f) {
	m_Drawable = std::make_shared<Util::Animation>(m_AnimationPaths, false, 500, false, 0);
	this->SetZIndex(12);
	SetLooping(true);
}

void Animation::Update(float deltaTime) {
	m_ElapsedTime += deltaTime;
	LOG_DEBUG("time = {}", m_ElapsedTime);
	// 依赖于 deltaTime 来控制动画速度
	if (m_ElapsedTime >= (1.0f / 60.0f)) {
		m_ElapsedTime = 0.0f;
		m_CurrentFrame = (m_CurrentFrame + 1) % m_AnimationPaths.size();
	}
}

bool Animation::IfAnimationEnds() const {
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	return animation && animation->GetCurrentFrameIndex() == animation->GetFrameCount() - 1;
}

void Animation::PlayAnimation(bool play) {
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	if (animation && animation->GetState() != Util::Animation::State::PLAY && play) {
		animation->Play();  // 開始播放動畫
	} else {
		animation->Pause();
	}
}

