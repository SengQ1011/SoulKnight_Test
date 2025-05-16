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
		const std::string& imagePath, float speed, int numRebound, bool canReboundBySword  = true, bool isBubble = false, bool bubbleTrail = false, const std::string &bubbleImagePath = "");
	~Projectile() override = default;

	//================ 提醒 ==============//
	void Init() override;
	void UpdateObject(float deltaTime) override;

	std::string GetName() const override { return "Projectile"; }

	void OnEventReceived(const EventInfo &eventInfo) override;

	// 靜態資源池，只載入一次同一圖片，資源共用（圖片資源共享）
	static std::unordered_map<std::string, std::shared_ptr<Util::Image>> sharedImages;

	//----Getter----
	[[nodiscard]] float GetSpeed() const{ return m_speed; };
	[[nodiscard]]int GetNumRebound() const{ return m_numRebound; };
	[[nodiscard]]int GetReboundCounter() const{ return m_reboundCounter; }
	[[nodiscard]] bool GetCanReboundBySword() const{ return m_canReboundBySword; };

	//----Setter----
	void SetImage(const std::string& imagePath);
	void ReflectChangeAttackCharacterType(CharacterType type);
	void SetSpeed(const float speed) { m_speed = speed; }
	void SetNumRebound(const int numRebound) { m_numRebound = numRebound; }
	void AddReboundCounter() { m_reboundCounter++; }
	void SetIsBubble(const bool yes) { m_isBubble = yes; }
	void ResetAll(const CharacterType type, const Util::Transform &bulletTransform, glm::vec2 direction, float size, int damage, const std::string& ImagePath, float speed,
					int numRebound, bool canReboundBySword, bool isBubble, bool bubbleTrail, const std::string &bubbleImagePath);


	void CreateBubbleBullet(const glm::vec2& pos, const glm::vec2& bulletDirection) const;

protected:
	std::string m_imagePath;
	glm::vec2 m_startPosition;
	float m_speed;
	int m_numRebound = 0;
	int m_reboundCounter = 0;
	bool m_canReboundBySword = false;
	bool m_isBubble = false;

	// 是否啟用泡泡尾跡
	bool m_enableBubbleTrail = false;
	// 泡泡間隔時間與計時器
	float m_bubbleSpawnInterval = 0.2f;  // 每0.2秒生成一次
	float m_bubbleTimer = 0.0f;
	// 泡泡子彈的屬性
	std::string m_bubbleImagePath;
	float m_bubbleSize = 4.0f;
	int m_bubbleDamage = 2;
	float m_bubbleSpeed = 3.0f;
	float m_bubbleStayTime = 3.0f;
};

#endif //PROJECTILE_HPP