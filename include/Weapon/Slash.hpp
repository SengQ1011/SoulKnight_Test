//
// Created by tjx20 on 4/10/2025.
//

#ifndef SLASH_HPP
#define SLASH_HPP
#include <memory>

#include "Animation.hpp"

class Slash final : public nGameObject {
public:
	explicit Slash(const CharacterType type, const std::shared_ptr<Animation> &anim, const float range);
	~Slash() = default;

	void Init();
	void Update() override;

private:
	CharacterType m_type;
	std::shared_ptr<Animation> m_animation;
	float m_attackRange;

};
#endif //SLASH_HPP
