//
// Created by tjx20 on 4/18/2025.
//

#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Attack/Attack.hpp"
namespace Util{ class Image; }

class Projectile : public Attack {
public:
	explicit Projectile(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction, float size, int damage,
		const std::string& imagePath, float speed, int numRebound);
	~Projectile() override = default;

	//================ 提醒 ==============//
	void Init() override;
	void UpdateObject(float deltaTime) override;

	void OnEventReceived(const EventInfo &eventInfo) override;

	// 靜態資源池，只載入一次同一圖片，資源共用（圖片資源共享）
	static std::unordered_map<std::string, std::shared_ptr<Util::Image>> sharedImages;

	//----Getter----
	[[nodiscard]] float GetSpeed() const{ return m_speed; };
	[[nodiscard]]int GetNumRebound() const{ return m_numRebound; };
	[[nodiscard]]int GetReboundCounter() const{ return m_reboundCounter; }

	//----Setter----
	void SetImage(const std::string& imagePath);
	void SetSpeed(const float speed) { m_speed = speed; };
	void ResetAll(const CharacterType type, const Util::Transform &bulletTransform, glm::vec2 direction, float size, int damage, const std::string& ImagePath, float speed, int numRebound);
	void OnCollision(const CollisionEventInfo &info);

protected:
	std::string m_imagePath;
	float m_speed;
	glm::vec2 m_startPosition;
	int m_numRebound = 0;
	int m_reboundCounter = 0;
};

#endif //PROJECTILE_HPP