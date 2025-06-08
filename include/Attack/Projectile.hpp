//
// Created by tjx20 on 4/18/2025.
//

#ifndef PROJECTILE_HPP
#define PROJECTILE_HPP

#include "Attack/Attack.hpp"

struct EffectAttackInfo;
namespace Util{ class Image; }

struct ProjectileInfo : public AttackInfo
{
	~ProjectileInfo() override = default;
	std::string imagePath = "";
	float speed = 50.0;
	int numRebound = 0;
	bool canReboundBySword  = true;
	bool canTracking = false;
	std::weak_ptr<nGameObject> target;
	bool isBubble = false;
	bool bubbleTrail = false;
	std::string bubbleImagePath = "";

	// chainAttack記錄
	int chainProjectionNum = 0;
	bool AllRound = false;
};

class Projectile : public Attack {
public:
	explicit Projectile(const ProjectileInfo& projectileInfo);
	~Projectile() override = default;

	//================ 提醒 ==============//
	void Init() override;
	void UpdateObject(float deltaTime) override;

	std::string GetName() const override { return "Projectile"; }

	// 靜態資源池，只載入一次同一圖片，資源共用（圖片資源共享）
	static std::unordered_map<std::string, std::shared_ptr<Util::Image>> sharedImages;

	//----Getter----
	[[nodiscard]] float GetSpeed() const{ return m_speed; };
	[[nodiscard]] int GetNumRebound() const{ return m_numRebound; };
	[[nodiscard]] int GetReboundCounter() const{ return m_reboundCounter; }
	[[nodiscard]] bool GetCanReboundBySword() const{ return m_canReboundBySword; };
	[[nodiscard]] bool GetIsBubble() const{ return m_isBubble; }

	//----Setter----
	void SetImage(const std::string& imagePath);
	void ReflectChangeAttackCharacterType(CharacterType type);
	void SetSpeed(const float speed) { m_speed = speed; }
	void SetNumRebound(const int numRebound) { m_numRebound = numRebound; }
	void AddReboundCounter() { m_reboundCounter++; }
	void SetIsBubble(const bool yes) { m_isBubble = yes; }
	void ResetAll(const ProjectileInfo& projectileInfo);

	void CreateBubbleBullet(const glm::vec2& pos, const glm::vec2& bulletDirection) const;

protected:
	std::string m_imagePath;
	glm::vec2 m_startPosition;
	float m_speed;
	int m_numRebound = 0;
	int m_reboundCounter = 0;
	bool m_canReboundBySword = false;
	bool m_canTracking = false;
	bool m_isBubble = false;
	bool m_enableBubbleTrail = false;		// 是否啟用泡泡尾跡
	std::string m_bubbleImagePath;

	// 子彈追蹤
	std::weak_ptr<nGameObject> m_Target;	// 此物件會跟隨目標旋轉
	float m_bezierTime = 0.0f;				// 貝茲曲線的插值參數：範圍 [0, 0.25]

	// 泡泡間隔時間與計時器
	float m_bubbleSpawnInterval = 0.2f;  // 每0.2秒生成一次
	float m_bubbleTimer = 0.0f;
	// 泡泡子彈的屬性
	float m_bubbleSize = 4.0f;
	int m_bubbleDamage = 2;
	float m_bubbleSpeed = 20.0f;
	float m_bubbleStayTime = 3.0f;

	// chainAttack記錄
	int m_chainProjectionNum = 0;
	bool m_allRound = false;
};

#endif //PROJECTILE_HPP