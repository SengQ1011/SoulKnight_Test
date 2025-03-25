//
// Created by tjx20 on 3/21/2025.
//

#ifndef ANIMATIONCOMPONENT_HPP
#define ANIMATIONCOMPONENT_HPP

#include <memory>
#include <unordered_map>
#include "Animation.hpp"
#include "Components/Component.hpp"

class AnimationComponent: public Component {
public:
	explicit AnimationComponent(std::unordered_map<State, std::shared_ptr<Animation>> animations);
	~AnimationComponent() override = default;
	void SetAnimation(State state);
	void Init() override;

	[[nodiscard]] std::shared_ptr<Animation> GetCurrentAnimation() const { return m_currentAnimation; }

private:
	std::unordered_map<State, std::shared_ptr<Animation>> m_Animations;  // 狀態對應動畫
	std::shared_ptr<Animation> m_currentAnimation;							// 現在的動畫
};

#endif //ANIMATIONCOMPONENT_HPP
