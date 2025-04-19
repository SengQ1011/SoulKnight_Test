//
// Created by tjx20 on 4/18/2025.
//

#ifndef ATTACK_HPP
#define ATTACK_HPP

#include "Override/nGameObject.hpp"
#include "Util/Image.hpp"

class Character;
class Attack : public nGameObject {
public:
	explicit Attack(const CharacterType type, const Util::Transform &attackTransform,
		const glm::vec2 direction, float size, int damage);
	~Attack() override= default;

	virtual void Init() = 0;
	virtual void UpdateObject(float deltaTime) = 0;

	//----Getter----
	glm::vec2 GetWorldPosition() const { return m_Transform.translation; }
	int GetDamage() const {return m_damage;}
	bool ShouldRemove() const { return m_markRemove; }

	//----Setter----
	void MarkForRemoval() { m_markRemove = true; }

protected:
	CharacterType m_type;
	glm::vec2 m_direction;		// 方向
	bool m_markRemove = false;		// 標記是否因碰撞需移除
	float m_size;
	int m_damage;
};

#endif //ATTACK_HPP
