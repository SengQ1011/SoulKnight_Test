//
// Created by tjx20 on 3/26/2025.
//

#ifndef BULLET_HPP
#define BULLET_HPP

#include "Override/nGameObject.hpp"
#include "Util/Image.hpp"

class Bullet : public nGameObject {
public:
	Bullet(const std::string& imagePath,Util::Transform bulletTransform,glm::vec2 direction, float speed, int damage);
	~Bullet() override= default;

	void Update(float deltaTime);
	//----Getter----
	glm::vec2 GetWorldPosition() const { return m_Transform.translation; }
	int GetDamage() const {return m_damage;}
	bool isOutOfBounds() const;

	//----Setter----
	void SetImage(const std::string& imagePath);

private:
	std::string m_imagePath;
	glm::vec2 m_direction;		// 方向
	float m_speed;
	int m_damage;
};

#endif //BULLET_HPP
