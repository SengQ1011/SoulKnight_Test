//
// Created by tjx20 on 3/26/2025.
//

#ifndef BULLET_HPP
#define BULLET_HPP

#include "Override/nGameObject.hpp"
#include "Util/Image.hpp"

class Bullet : public nGameObject {
public:
	Bullet(const CharacterType type, const std::string& imagePath, const Util::Transform &bulletTransform,
		glm::vec2 direction, float size, float speed, int damage, int numRebound);
	~Bullet() override= default;

	// 靜態資源池，只載入一次同一圖片
	static std::unordered_map<std::string, std::shared_ptr<Util::Image>> sharedImages;

	// Init
	void PostInitialize();
	void UpdateLocation(float deltaTime);

	//----Getter----
	glm::vec2 GetWorldPosition() const { return m_Transform.translation; }
	int GetDamage() const {return m_damage;}
	bool ShouldRemove() const { return m_hasHit; }

	//----Setter----
	void SetImage(const std::string& imagePath);
	void MarkForRemoval() { m_hasHit = true; }
	void ResetAll(const CharacterType type, const std::string& ImagePath, const Util::Transform &bulletTransform, glm::vec2 direction, float size, float speed, int damage, int numRebound);

	void onCollision(const std::shared_ptr<nGameObject> &other, CollisionInfo &info) override;

private:
	CharacterType m_type;
	std::string m_imagePath;
	glm::vec2 m_direction;		// 方向
	glm::vec2 m_startPosition;
	float m_size;
	float m_speed;
	int m_damage;
	bool m_hasHit = false; // 標記是否因碰撞需移除
	int m_numRebound = 0;
	int m_reboundCounter = 0;
};

#endif //BULLET_HPP
