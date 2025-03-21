//
// Created by tjx20 on 3/12/2025.
//

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Override/nGameObject.hpp"
#include <vector>
#include <string>
#include <memory>
#include "Util/Animation.hpp"

class Animation: public nGameObject {
public:
	Animation(std::vector<std::string> AnimationPaths);
	~Animation() = default;

	// 禁用拷贝构造和拷贝赋值
	Animation(const Animation&) = delete;
	Animation& operator=(const Animation&) = delete;


	void Update(float deltaTime);

	[[nodiscard]] bool IsLooping() const {
		auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
		return animation ? animation->GetLooping() : false;
	}

	[[nodiscard]] bool IsPlaying() const {
		auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
		return animation && animation->GetState() == Util::Animation::State::PLAY;
	}

	void SetLooping(bool looping) {
		auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
		if (animation) animation->SetLooping(looping);
	}

	[[nodiscard]] bool IfAnimationEnds() const;
	void PlayAnimation(bool play);

private:
	std::vector<std::string> m_AnimationPaths;  // 動畫幀列表
	float m_ElapsedTime;                        // 累積時間
};

#endif //ANIMATION_HPP