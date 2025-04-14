//
// Created by tjx20 on 3/14/2025.
//

#include "Animation.hpp"
#include "Util/Logger.hpp"

Animation::Animation(std::vector<std::string> AnimationPaths, bool needLoop)
	: m_AnimationPaths(std::move(AnimationPaths)), m_Looping(needLoop) {
	m_Drawable = std::make_shared<Util::Animation>(m_AnimationPaths, false, 500, false, 0);
	this->SetZIndexType(ZIndexType::CUSTOM);
	SetLooping(m_Looping);
}

bool Animation::IfAnimationEnds() const {
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	return animation &&
		   !animation->GetLooping() &&                                 // 確保不是循環動畫
		   animation->GetState() == Util::Animation::State::ENDED;      // 播放狀態為 ENDED（代表播放完畢）
}


// 自动播放（内部控制更新频率）
void Animation::PlayAnimation(bool play) {
	auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
	if(m_AnimationPaths.size() >= 5) animation->SetInterval(1000.0f / 30.0f);
	else animation->SetInterval(1000.0f / 5.0f);

	if (animation && animation->GetState() != Util::Animation::State::PLAY && play) {
		animation->Play();  // 開始播放動畫
	} else {
		animation->Pause();
	}
}

