//
// Created by tjx20 on 4/18/2025.
//

#include "Attack/Projectile.hpp"

#include "Attack/AttackManager.hpp"
#include "Components/CollisionComponent.hpp"
#include "Creature/Character.hpp"
#include "Scene/SceneManager.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Projectile::Projectile(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction,
					   float size, int damage, const std::string &imagePath, float speed, int numRebound, bool canReboundBySword, bool isBubble, bool bubbleTrail, const std::string &bubbleImagePath)
						   : Attack(type, attackTransform, direction, size, damage),
								m_imagePath(imagePath), m_speed(speed), m_numRebound(numRebound), m_canReboundBySword(canReboundBySword),
								m_isBubble(isBubble), m_enableBubbleTrail(bubbleTrail), m_bubbleImagePath(bubbleImagePath) {}

//=========================== (實作) ================================//
// 靜態成員變數
std::unordered_map<std::string, std::shared_ptr<Util::Image>> Projectile::sharedImages;

void Projectile::Init() {
	// 明確設定世界坐標（從傳入的 Transform 取得）
	this->m_WorldCoord = m_Transform.translation;
	m_startPosition = this->m_WorldCoord;
	// 其他初始化（縮放、圖片等）
	this->SetInitialScale(m_Transform.scale);
	this->SetZIndexType(ZIndexType::ATTACK);
	SetImage(m_imagePath);

	// 設定碰到就消失
	m_disappearOnHit = true;

	auto CollisionComp = this->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!CollisionComp) { CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION); }
	CollisionComp->ResetCollisionMask();

	//設置觸發器 和 觸發事件
	CollisionComp->SetTrigger(true);
	CollisionComp->SetTriggerStrategy(std::make_unique<AttackTriggerStrategy>(m_damage));

	if(m_type == CharacterType::PLAYER) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Player_Projectile);
		CollisionComp->AddCollisionMask(CollisionLayers_Enemy);
	}
	else if (m_type == CharacterType::ENEMY) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Enemy_Projectile);
		CollisionComp->AddCollisionMask(CollisionLayers_Player);
	}
	CollisionComp->AddCollisionMask(CollisionLayers_Terrain);

	// TODO:測試子彈大小
	CollisionComp->SetSize(glm::vec2(m_size));
	// auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	// currentScene->GetRoot().lock()->AddChild(CollisionComp->GetVisibleBox());
	// currentScene->GetCamera().lock()->AddChild(CollisionComp->GetVisibleBox());
}

void Projectile::UpdateObject(const float deltaTime) {
	// 讓子彈按方向移動
	this->m_WorldCoord += m_direction * m_speed * deltaTime;

	if (m_isBubble) {
		m_bubbleStayTime -= deltaTime;
		if (m_bubbleStayTime <= 0.0f) {
			m_bubbleStayTime = 3.0f;
			this->MarkForRemoval();
		}
	}
	if (m_enableBubbleTrail) {
		m_bubbleTimer += deltaTime;
		if (m_bubbleTimer >= m_bubbleSpawnInterval) {
			m_bubbleTimer = 0.0f;

			// 左右側相對於方向向量垂直偏移
			glm::vec2 leftOffset = glm::normalize(glm::vec2(-m_direction.y, m_direction.x)) * 5.0f;
			glm::vec2 rightOffset = glm::normalize(glm::vec2(m_direction.y, -m_direction.x)) * 5.0f;

			glm::vec2 leftPos = m_WorldCoord+ leftOffset;
			glm::vec2 rightPos = m_WorldCoord + rightOffset;

			// 主子彈方向
			const glm::vec2 forward = m_direction;
			// 泡泡的左右方向是 forward 的垂直方向
			const glm::vec2 leftDir = glm::normalize(glm::vec2(-forward.y, forward.x));
			const glm::vec2 rightDir = glm::normalize(glm::vec2(forward.y, -forward.x));

			// 创建泡泡子弹 (滞留型攻击)
			CreateBubbleBullet(leftPos, leftDir);
			CreateBubbleBullet(rightPos, rightDir);
		}
	}
	if(glm::distance(this->m_WorldCoord, m_startPosition) >= 1000.0f)
	{
		this->MarkForRemoval();
	}
}

void Projectile::OnEventReceived(const EventInfo &eventInfo)
{
	if (eventInfo.GetEventType() != EventType::Collision) return;

	const auto& collisionEvent = static_cast<const CollisionEventInfo&>(eventInfo);
	OnCollision(collisionEvent);
}


void Projectile::ResetAll(const CharacterType type, const Util::Transform &attackTransform, glm::vec2 direction, float size, int damage,
	const std::string& ImagePath, float speed, int numRebound, bool canReboundBySword, bool isBubble, bool bubbleTrail, const std::string &bubbleImagePath) {
	m_type = type;
	m_imagePath = ImagePath;
	this->m_Transform = attackTransform;
	this->m_direction = direction;
	this->m_size = size;
	m_speed = speed;
	m_damage = damage;
	m_numRebound = numRebound;
	m_reboundCounter = 0;
	m_markRemove = false;
	m_canReboundBySword = canReboundBySword;
	m_isBubble = isBubble;
	m_enableBubbleTrail = bubbleTrail;
	m_bubbleImagePath = bubbleImagePath;
}


void Projectile::SetImage(const std::string& imagePath) {
	m_imagePath = imagePath;
	if (sharedImages.find(imagePath) == sharedImages.end()) {
		sharedImages[imagePath] = std::make_shared<Util::Image>(imagePath);
	}
	m_Drawable = sharedImages[imagePath];
}

// 自身碰撞 就不是Trigger的事了 因爲Trigger是用來觸發事件的
void Projectile::OnCollision(const CollisionEventInfo &info) {
	const auto& other = info.GetObjectB();
	bool hitTarget = false;

	if (const auto& character = std::dynamic_pointer_cast<Character>(other)) {
		hitTarget = (m_type != character->GetType());
	}

	// 可被反彈
	if (m_canReboundBySword)
	{

	}

	// 可反彈
	if (m_numRebound > 0 && m_reboundCounter < m_numRebound && !hitTarget) {
		const glm::vec2 collisionNormal = info.GetCollisionNormal();
		const float dotProduct = glm::dot(m_direction, collisionNormal);
		m_direction = glm::normalize(m_direction - 2.0f * dotProduct * collisionNormal);

		m_Transform.rotation = glm::atan(m_direction.y, m_direction.x);
		m_reboundCounter++;
	}else MarkForRemoval();

}

void Projectile::CreateBubbleBullet(const glm::vec2& pos, const glm::vec2& bulletDirection)
{
	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = pos;									// 子彈的位置
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x);        // 子彈的角度

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	// 延緩發射
	attackManager->EnqueueSpawn([=]() {
		attackManager->spawnProjectile(CharacterType::ENEMY, bulletTransform, bulletDirection, m_bubbleSize, m_bubbleDamage, m_bubbleImagePath, m_bubbleSpeed, 0, false, true, false, "");
	});
}

