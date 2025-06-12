//
// Created by tjx20 on 3/12/2025.
//

#ifndef ANIMATION_HPP
#define ANIMATION_HPP

#include "Override/nGameObject.hpp"
#include "Util/Animation.hpp"

class Animation : public nGameObject
{
public:
	Animation(const std::vector<std::string> &AnimationPaths, bool needLoop, const std::string &_class = "Animation",
			  float interval = 0);
	~Animation() override = default;

	// 禁用拷贝构造和拷贝赋值
	Animation(const Animation &) = delete;
	Animation &operator=(const Animation &) = delete;

	[[nodiscard]] bool IsLooping() const
	{
		auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
		return animation ? animation->GetLooping() : false;
	}

	[[nodiscard]] bool IsPlaying() const
	{
		auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
		return animation && animation->GetState() == Util::Animation::State::PLAY;
	}

	void SetLooping(bool looping)
	{
		auto animation = std::dynamic_pointer_cast<Util::Animation>(m_Drawable);
		if (animation)
			animation->SetLooping(looping);
	}

	[[nodiscard]] bool IfAnimationEnds() const;
	void PlayAnimation(bool play);

private:
	std::vector<std::string> m_AnimationPaths; // 動畫幀列表
	bool m_Looping;
};

#endif // ANIMATION_HPP
