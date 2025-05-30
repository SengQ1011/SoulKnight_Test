//
// Created by tjx20 on 4/18/2025.
//

#include "Attack/Projectile.hpp"

#include <complex>

#include "Attack/AttackManager.hpp"
#include "Components/CollisionComponent.hpp"
#include "Components/ProjectileComponent.hpp"
#include "Creature/Character.hpp"
#include "ImagePoolManager.hpp"
#include "Scene/SceneManager.hpp"
#include "TriggerStrategy/AttackTriggerStrategy.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"

Projectile::Projectile(const ProjectileInfo& projectileInfo)
						   : Attack(projectileInfo), m_startPosition(projectileInfo.attackTransform.translation),
								m_imagePath(projectileInfo.imagePath), m_speed(projectileInfo.speed),
								m_numRebound(projectileInfo.numRebound), m_canReboundBySword(projectileInfo.canReboundBySword),
								m_canTracking(projectileInfo.canTracking), m_Target(projectileInfo.target),
								m_isBubble(projectileInfo.isBubble), m_enableBubbleTrail(projectileInfo.bubbleTrail),
								m_bubbleImagePath(projectileInfo.bubbleImagePath), m_chainProjectionNum(projectileInfo.chainProjectionNum) {}

//=========================== (實作) ================================//
void Projectile::Init() {
	// 明確設定世界坐標（從傳入的 Transform 取得）
	this->m_WorldCoord = m_Transform.translation;
	m_startPosition = this->m_WorldCoord;
	// 全部子彈太大了，縮小30%
	this->m_Transform.scale = glm::vec2(0.7f, 0.7f);
	// 其他初始化（縮放、圖片等）
	this->SetInitialScale(m_Transform.scale);
	this->SetZIndexType(ZIndexType::ATTACK);
	SetImage(m_imagePath);

	// 加入子彈類機制組件
	if (auto ProjectileComp = this->GetComponent<ProjectileComponent>(ComponentType::PROJECTILE); !ProjectileComp)
	{
		ProjectileComp = this->AddComponent<ProjectileComponent>(ComponentType::PROJECTILE);
	}

	// 加入碰撞組件
	auto CollisionComp = this->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	if (!CollisionComp) { CollisionComp = this->AddComponent<CollisionComponent>(ComponentType::COLLISION); }
	CollisionComp->ResetCollisionMask();

	//設置觸發器 和 觸發事件
	CollisionComp->ClearTriggerStrategies();
	CollisionComp->SetTrigger(true);
	CollisionComp->AddTriggerStrategy(std::make_unique<AttackTriggerStrategy>(m_damage, m_elementalDamage));

	if(m_type == CharacterType::PLAYER) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Player_Projectile);
		CollisionComp->AddCollisionMask(CollisionLayers_Enemy);
	}
	else if (m_type == CharacterType::ENEMY) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Enemy_Projectile);
		CollisionComp->AddCollisionMask(CollisionLayers_Player);
	}
	CollisionComp->AddCollisionMask(CollisionLayers_Terrain);
	CollisionComp->SetSize(glm::vec2(m_size));

	// TODO:測試子彈大小
	// auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	// currentScene->GetRoot().lock()->AddChild(CollisionComp->GetVisibleBox());
	// currentScene->GetCamera().lock()->SafeAddChild(CollisionComp->GetVisibleBox());
}


void Projectile::UpdateObject(const float deltaTime) {
	if (m_isBubble) {
		m_bubbleStayTime -= deltaTime;
		// 泡泡到期消失
		if (m_bubbleStayTime <= 0.0f) {
			this->MarkForRemoval();
			return;
		}

		// 使用 sin 控制速度（平滑遞減）
		float t = 3.0f - m_bubbleStayTime;
		float T = 3.0f;
		float percent = std::clamp(t / T, 0.0f, 1.0f);
		m_speed = m_bubbleSpeed * sinf((1.0f - percent) * (3.1415926f / 2.0f));

		// 移動泡泡
		this->m_WorldCoord += m_direction * m_speed * deltaTime;
	}
	else if(m_canTracking && !m_Target.expired())
	{
		// 方法一：追蹤導彈（貝茲曲線）
		auto targetPosition = m_Target.lock()->GetWorldCoord();
		glm::vec2 P0 = m_WorldCoord;
		glm::vec2 P2 = targetPosition;

		m_bezierTime += deltaTime * 0.15f;
		m_bezierTime = glm::clamp(m_bezierTime, 0.0f, 0.25f);
		// 控制點P1:
		// 數值越大： 導彈轉彎越平緩，反應比較慢，感覺像是
		// 數值越小： 導彈彎得越急促，更快鎖定目標
		float curveDistance = 100.0f;
		glm::vec2 P1 = P0 + m_direction * curveDistance;

		glm::vec2 bezierPos = (1 - m_bezierTime) * (1 - m_bezierTime) * P0 +
							  2 * (1 - m_bezierTime) * m_bezierTime * P1 +
							  m_bezierTime * m_bezierTime * P2;

		glm::vec2 newDir = glm::normalize(bezierPos - m_WorldCoord);

		// 方法二：動態尋路法
		// glm::vec2 toTarget = glm::normalize(targetPosition - m_WorldCoord);
		// // 越大越容易轉彎
		// float curveDistance = 50.0f;
		// // 線性插值方向
		// float turnRate = curveDistance * deltaTime;
		// glm::vec2 newDir = glm::normalize(glm::mix(m_direction, toTarget, turnRate));

		m_direction = newDir;
		m_Transform.rotation = atan2(newDir.y, newDir.x);

		// 移動
		m_WorldCoord += m_direction * m_speed * deltaTime;


	}
	else
		this->m_WorldCoord += m_direction * m_speed * deltaTime;

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


void Projectile::ReflectChangeAttackCharacterType(CharacterType type)
{
	m_type = type;
	auto CollisionComp = this->GetComponent<CollisionComponent>(ComponentType::COLLISION);
	CollisionComp->ResetCollisionMask();
	if(m_type == CharacterType::PLAYER) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Player_Projectile);
		CollisionComp->AddCollisionMask(CollisionLayers_Enemy);
	}
	else if (m_type == CharacterType::ENEMY) {
		CollisionComp->SetCollisionLayer(CollisionLayers_Enemy_Projectile);
		CollisionComp->AddCollisionMask(CollisionLayers_Player);
	}
	CollisionComp->AddCollisionMask(CollisionLayers_Terrain);
}


void Projectile::ResetAll(const ProjectileInfo& projectileInfo) {
	m_type = projectileInfo.type;
	m_Transform = projectileInfo.attackTransform;
	m_direction = projectileInfo.direction;
	m_size = projectileInfo.size;
	m_damage = projectileInfo.damage;
	m_elementalDamage = projectileInfo.elementalDamage;
	m_chainAttack = projectileInfo.chainAttack;

	m_imagePath = projectileInfo.imagePath;
	m_speed = projectileInfo.speed;
	m_numRebound = projectileInfo.numRebound;
	m_canReboundBySword = projectileInfo.canReboundBySword;
	m_canTracking = projectileInfo.canTracking;
	m_Target = projectileInfo.target;
	m_isBubble = projectileInfo.isBubble;
	m_enableBubbleTrail = projectileInfo.bubbleTrail;
	m_bubbleImagePath = projectileInfo.bubbleImagePath;

	m_bezierTime = 0;
	m_reboundCounter = 0;
	m_markRemove = false;
	m_bubbleTimer = 0.0f;
	m_bubbleStayTime = 3.0f;

	m_chainProjectionNum = projectileInfo.chainProjectionNum;
}


void Projectile::SetImage(const std::string& imagePath) {
	m_Drawable = ImagePoolManager::GetInstance().GetImage(imagePath);
}

void Projectile::CreateBubbleBullet(const glm::vec2& pos, const glm::vec2& bulletDirection) const
{
	// 建立 Transform
	Util::Transform bulletTransform;
	bulletTransform.translation = pos;									// 子彈的位置
	bulletTransform.rotation = glm::atan(bulletDirection.y, bulletDirection.x);        // 子彈的角度

	ProjectileInfo bubbleInfo;
	bubbleInfo.type = this->m_type;
	bubbleInfo.attackTransform = bulletTransform;
	bubbleInfo.direction = bulletDirection;
	bubbleInfo.size = this->m_bubbleSize;
	bubbleInfo.damage = this->m_bubbleDamage;

	bubbleInfo.imagePath = this->m_bubbleImagePath;
	bubbleInfo.speed = this->m_bubbleSpeed;
	bubbleInfo.numRebound = 0;
	bubbleInfo.canReboundBySword = false;
	bubbleInfo.isBubble = true;
	bubbleInfo.bubbleTrail = false;

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	// 延緩發射
	attackManager->EnqueueSpawn([=]() {
		attackManager->spawnProjectile(bubbleInfo);
	});
}