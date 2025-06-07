//
// Created by tjx20 on 4/25/2025.
//

#include "Components/EnemyAI/AttackStrategy.hpp"

#include <glm/gtx/rotate_vector.hpp>
#include "Attack/AttackManager.hpp"
#include "Attack/Projectile.hpp"
#include "Components/AiComponent.hpp"
#include "Components/AttackComponent.hpp"
#include "Creature/Character.hpp"
#include "RandomUtil.hpp"
#include "Scene/SceneManager.hpp"
#include "StructType.hpp"

#include "Factory/CharacterFactory.hpp"
#include "Room/MonsterRoom.hpp"
#include "Weapon/Weapon.hpp"

bool MeleeAttack::CanAttack(const EnemyContext &ctx)
{
	const auto aiComp = ctx.GetAIComp();
	if (const auto target = aiComp->GetTarget().lock(); target != nullptr)
	{
		const auto distance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		return distance < m_meleeAttackDistance;
	}
	return false;
}

void MeleeAttack::Update(const EnemyContext &ctx, const float deltaTime)
{
	if (const auto aiComp = ctx.GetAIComp(); aiComp->GetReadyAttackTimer() <= 0.0f)
	{
		aiComp->HideReadyAttackIcon();
		ctx.attackComp->TryAttack();
		aiComp->SetEnemyState(enemyState::IDLE);
		ctx.moveComp->SetDesiredDirection(glm::vec2{0.0f, 0.0f});
		aiComp->ResetReadyAttackTimer();
	}
}

bool GunAttack::CanAttack(const EnemyContext &ctx)
{
	const auto aiComp = ctx.GetAIComp();
	if (const auto target = aiComp->GetTarget().lock(); target != nullptr)
	{
		const auto distance = glm::distance(target->GetWorldCoord(), ctx.enemy->GetWorldCoord());
		return distance < m_gunAttackDistance && distance > m_gunAttackDistance * 0.3f;
	}
	return false;
}

void GunAttack::Update(const EnemyContext &ctx, const float deltaTime)
{
	if (const auto aiComp = ctx.GetAIComp(); aiComp->GetReadyAttackTimer() <= 0.0f)
	{
		aiComp->HideReadyAttackIcon();
		ctx.attackComp->TryAttack();
		aiComp->SetEnemyState(

		enemyState::IDLE);
		aiComp->ResetReadyAttackTimer();
	}
}

bool CollisionAttack::CanAttack(const EnemyContext &ctx)
{
	return false;
}


//--------------------------------------------
// BossAttackStrategy
//--------------------------------------------
bool BossAttackStrategy::CanAttack(const EnemyContext &ctx)
{
	// Boss的攻擊判斷完全由移動策略控制，這裡只負責執行
	return true;
}

void BossAttackStrategy::Update(const EnemyContext& ctx, const float deltaTime)
{
	// 根據當前的enemyState設置對應的技能
	auto currentState = ctx.GetAIComp()->GetEnemyState();

	// 判斷是否為技能狀態
	if (currentState >= enemyState::SKILL1 && currentState <= enemyState::SKILL5) {
		currentSkill = static_cast<BossSkillType>(static_cast<int>(currentState) - static_cast<int>(enemyState::SKILL1));
		auto& currentSkillInfo = skills[currentSkill];
		if(currentSkillInfo.isActive ==  false)
		{
			currentSkillInfo.isActive = true;
			currentSkillInfo.currentPhase = 0;
			currentSkillInfo.activeTimer = currentSkillInfo.duration;
			// 設置技能持續時間
			ctx.GetAIComp()->SetSkillTimer(skills[currentSkill].duration);
		}
	}

	auto& currentSkillInfo = skills[currentSkill];
	if (currentSkillInfo.isActive) {
		currentSkillInfo.activeTimer -= deltaTime;
		if (currentSkillInfo.activeTimer <= 0) {
			currentSkillInfo.isActive = false;
			currentSkillInfo.activeTimer = 0.0f;
			ctx.GetAIComp()->SetSkillTimer(0.0f);  // 重置技能計時器
		} else {
			ExecuteCurrentSkill(ctx);
		}
	}
}

void BossAttackStrategy::ExecuteCurrentSkill(const EnemyContext& ctx)
{
	switch (currentSkill) {
		case BossSkillType::SKILL1:
			ExecuteSkill1(ctx);
			break;
		case BossSkillType::SKILL2:
			ExecuteSkill2(ctx);
			break;
		case BossSkillType::SKILL3:
			ExecuteSkill3(ctx);
			break;
		case BossSkillType::SKILL4:
			ExecuteSkill4(ctx);
			break;
		case BossSkillType::SKILL5:
			ExecuteSkill5(ctx);
			break;
	}
}

void BossAttackStrategy::ExecuteSkill1(const EnemyContext& ctx)
{
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	auto& skillInfo = skills[currentSkill];

	// 每 2秒發射一次，共 3 次（duration 通常要 ≥ 1.5 秒）
	const int totalPhases = 3;
	const float phaseInterval = 2.0f;

	// 計算這個 phase 應該觸發的時間點（倒數計時）
	float expectedTriggerTime = skillInfo.duration - skillInfo.currentPhase * phaseInterval;

	// 若已達成該 phase 的觸發時機
	if (skillInfo.currentPhase < totalPhases && skillInfo.activeTimer <= expectedTriggerTime) {
		glm::vec2 direction = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
		SpawnLargeSnowball(ctx, direction);
		skillInfo.currentPhase++;
	}
}

void BossAttackStrategy::ExecuteSkill2(const EnemyContext& ctx)
{
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	auto& skillInfo = skills[currentSkill];
	const float fireInterval = 0.05f;
	const float handOffset = 8.0f;
	const float rotationSpeed = glm::two_pi<float>() / 1.333f;  // 一整圈時間 = 技能持續時間 → 完美形成一個漩渦

	float timeElapsed = skillInfo.duration - skillInfo.activeTimer;

	if (skillInfo.activeTimer <= skillInfo.duration - skillInfo.castTime - (skillInfo.currentPhase * fireInterval)) {

		// 根據時間旋轉
		float baseAngle = timeElapsed * rotationSpeed;

		// 兩隻手在 baseAngle 前後各偏移一個固定角度
		for (int i = 0; i < 2; ++i) {
			float offsetAngle = (i == 0) ? 0.0f : glm::pi<float>();  // 左右手相反方向
			float angle = baseAngle + offsetAngle;

			glm::vec2 direction = glm::vec2(cos(angle), sin(angle));
			glm::vec2 offset = direction * handOffset;
			SpawnSmallSnowball(ctx, direction, offset);
		}

		skillInfo.currentPhase++;
	}
}

void BossAttackStrategy::ExecuteSkill3(const EnemyContext& ctx)
{
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	auto& skillInfo = skills[currentSkill];
	// 只發射一次
	const int numBullets = 6;
	const float spreadAngle = glm::radians(60.0f);  // 總共散佈的角度
	const glm::vec2 baseDirection = glm::normalize(target->GetWorldCoord() - ctx.enemy->GetWorldCoord());
	const float baseRotation = glm::atan(baseDirection.y, baseDirection.x);

	if (skillInfo.currentPhase == 0 && skillInfo.activeTimer <= skillInfo.duration - skillInfo.castTime) {
		for (int i = 0; i < numBullets; ++i) {
			float angleOffset = spreadAngle * ((static_cast<float>(i) / (numBullets - 1)) - 0.5f);
			float newRotation = baseRotation + angleOffset;

			// 以旋轉角度轉回方向向量
			glm::vec2 newDirection = glm::vec2(cos(newRotation), sin(newRotation));
			SpawnLongBUllet(ctx, newDirection);
		}
		skillInfo.currentPhase++;
	}
}

void BossAttackStrategy::ExecuteSkill4(const EnemyContext& ctx)
{
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	auto& skillInfo = skills[currentSkill];
	if (skillInfo.currentPhase == 0 && skillInfo.activeTimer <= skillInfo.duration - skillInfo.castTime) {
		const int numIceSpikes = 5;

		glm::vec2 toTarget = target->GetWorldCoord() - ctx.enemy->GetWorldCoord();
		glm::vec2 baseDir = glm::normalize(toTarget);
		float baseAngle = glm::atan(baseDir.y, baseDir.x);  // 基準角：面向玩家

		const float angleStep = glm::radians(360.0f / numIceSpikes);  // 每條冰刺間隔角度

		for (int i = 0; i < numIceSpikes; ++i) {
			// 從 baseAngle 開始，順時針均分發射
			float angle = baseAngle + i * angleStep;
			glm::vec2 direction = glm::vec2(glm::cos(angle), glm::sin(angle));
			SpawnIceSpike(ctx, direction);
		}
		skillInfo.currentPhase++;
	}
}

void BossAttackStrategy::ExecuteSkill5(const EnemyContext& ctx)
{
	auto target = ctx.GetAIComp()->GetTarget().lock();
	if (!target) return;

	auto& skillInfo = skills[currentSkill];
	if (skillInfo.currentPhase < 5 &&
		skillInfo.activeTimer <= skillInfo.duration - (skillInfo.castTime * (skillInfo.currentPhase+1)) - (0.166f * skillInfo.currentPhase)) {
		SpawnShockwave(ctx);
		SpawnMiniSnowman(ctx, ctx.enemy->GetWorldCoord());
		skillInfo.currentPhase++;
	}
}

void BossAttackStrategy::SpawnLargeSnowball(const EnemyContext& ctx, const glm::vec2& direction)
{
	ProjectileInfo largeSnowBall;
	largeSnowBall.type = CharacterType::ENEMY;
	largeSnowBall.attackTransform.translation = ctx.enemy->GetWorldCoord();
	largeSnowBall.attackTransform.rotation = glm::atan(direction.y, direction.x);
	largeSnowBall.direction = direction;
	largeSnowBall.size = 16.0f;
	largeSnowBall.damage = 3;
	largeSnowBall.elementalDamage = StatusEffect::NONE;
	largeSnowBall.chainAttack.enabled = true;
	largeSnowBall.imagePath = std::string(RESOURCE_DIR) + "/attackUI/bullet/bullet2/bullet2_3.png";
	largeSnowBall.speed = 120.0f;
	largeSnowBall.canReboundBySword = false;
	largeSnowBall.canTracking = false;
	largeSnowBall.isBubble = false;
	largeSnowBall.bubbleTrail = false;
	largeSnowBall.bubbleImagePath = "";
	largeSnowBall.chainProjectionNum = 0;

	// 设置连锁攻击的小雪球
	auto smallSnowBall = std::make_shared<ProjectileInfo>();
	smallSnowBall->type = CharacterType::ENEMY;
	smallSnowBall->direction = glm::vec2(0.0f);
	smallSnowBall->size = 8.0f;
	smallSnowBall->damage = 2;
	smallSnowBall->elementalDamage = StatusEffect::FROZEN;
	smallSnowBall->chainAttack.enabled = false;
	smallSnowBall->imagePath = std::string(RESOURCE_DIR) + "/attackUI/bullet/bullet_111.png";
	smallSnowBall->speed = 100.0f;
	smallSnowBall->canReboundBySword = false;
	smallSnowBall->canTracking = false;
	smallSnowBall->isBubble = false;
	smallSnowBall->bubbleTrail = false;
	smallSnowBall->bubbleImagePath = "";
	smallSnowBall->chainProjectionNum = 10;

	largeSnowBall.chainAttack.attackType = AttackType::PROJECTILE;
	largeSnowBall.chainAttack.nextAttackInfo = smallSnowBall;

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnProjectile(largeSnowBall);
}

void BossAttackStrategy::SpawnSmallSnowball(const EnemyContext &ctx, const glm::vec2 &direction, const glm::vec2 &offset)
{
	ProjectileInfo smallSnowBall;
	smallSnowBall.type = CharacterType::ENEMY;
	smallSnowBall.attackTransform.translation = ctx.enemy->GetWorldCoord() + offset;
	smallSnowBall.direction = direction;
	smallSnowBall.size = 8.0f;
	smallSnowBall.damage = 2;
	smallSnowBall.elementalDamage = StatusEffect::FROZEN;
	smallSnowBall.chainAttack.enabled = false;
	smallSnowBall.imagePath = std::string(RESOURCE_DIR) + "/attackUI/bullet/bullet_111.png";
	smallSnowBall.speed = 100.0f;
	smallSnowBall.canReboundBySword = false;
	smallSnowBall.canTracking = false;
	smallSnowBall.isBubble = false;
	smallSnowBall.bubbleTrail = false;
	smallSnowBall.bubbleImagePath = "";
	smallSnowBall.chainProjectionNum = 0;

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnProjectile(smallSnowBall);
}

void BossAttackStrategy::SpawnLongBUllet(const EnemyContext &ctx, const glm::vec2 &direction)
{
	ProjectileInfo longBullet;
	longBullet.type = CharacterType::ENEMY;
	longBullet.attackTransform.translation = ctx.enemy->GetWorldCoord() + glm::vec2(0.0f, 8.0f);
	longBullet.attackTransform.rotation = glm::atan(direction.y, direction.x);
	longBullet.direction = direction;
	longBullet.size = 10.0f;
	longBullet.damage = 3;
	longBullet.elementalDamage = StatusEffect::NONE;
	longBullet.chainAttack.enabled = false;
	longBullet.imagePath = std::string(RESOURCE_DIR) + "/attackUI/bullet/bullet_87.png";
	longBullet.speed = 180.0f;
	longBullet.canReboundBySword = true;
	longBullet.canTracking = false;
	longBullet.isBubble = false;
	longBullet.bubbleTrail = false;
	longBullet.bubbleImagePath = "";
	longBullet.chainProjectionNum = 0;

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnProjectile(longBullet);
}

void BossAttackStrategy::SpawnIceSpike(const EnemyContext& ctx, const glm::vec2& direction)
{
	EffectAttackInfo iceSpike;
	iceSpike.type = CharacterType::ENEMY;
	iceSpike.attackTransform.translation = ctx.enemy->GetWorldCoord() + glm::vec2(0,-20.0f);
	iceSpike.direction = direction;
	iceSpike.size = 22.0f;
	iceSpike.damage = 2;
	iceSpike.elementalDamage = StatusEffect::FROZEN;
	iceSpike.chainAttack.enabled = true;

	iceSpike.canBlockingBullet = false;
	iceSpike.canReflectBullet = false;
	iceSpike.effectType = EffectAttackType::ICE_SPIKE;
	iceSpike.continuouslyExtending = true;
	iceSpike.intervalCreateChainAttack = 0.2f;

	// 淺複製
	static auto copyAttack = std::make_shared<EffectAttackInfo>();
	*copyAttack = iceSpike;

	iceSpike.chainAttack.attackType = AttackType::EFFECT_ATTACK;
	iceSpike.chainAttack.nextAttackInfo = copyAttack;

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnEffectAttack(iceSpike);
}

void BossAttackStrategy::SpawnShockwave(const EnemyContext& ctx)
{
	EffectAttackInfo shockwave;
	shockwave.type = CharacterType::ENEMY;
	shockwave.attackTransform.translation = ctx.enemy->GetWorldCoord() + glm::vec2(0,-10.0f);
	shockwave.attackTransform.rotation = 0;
	shockwave.direction = glm::vec2(0.0f, 0.0f);
	shockwave.size = 40.0f;
	shockwave.damage = 2;
	shockwave.elementalDamage = StatusEffect::NONE;
	shockwave.chainAttack.enabled = false;

	shockwave.canBlockingBullet = false;
	shockwave.canReflectBullet = false;
	shockwave.effectType = EffectAttackType::LARGE_SHOCKWAVE;
	shockwave.continuouslyExtending = false;

	const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock();
	const auto attackManager = currentScene->GetManager<AttackManager>(ManagerTypes::ATTACK);
	attackManager->spawnEffectAttack(shockwave);
}

void BossAttackStrategy::SpawnMiniSnowman(const EnemyContext& ctx, const glm::vec2& position)
{
	auto enemy = CharacterFactory::GetInstance().createEnemy(21);
	if (!enemy)
	{
		LOG_ERROR("can't create enemy");
		return;
	}
	if (const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock())
	{
		currentScene->GetRoot().lock()->AddChild(enemy);
		currentScene->GetCamera().lock()->SafeAddChild(enemy);
		enemy->SetRegisteredToScene(true);
		enemy->SetInitialScale(glm::vec2(0.8f, 0.8f));
		enemy->SetInitialScaleSet(true);
		enemy->m_WorldCoord = ctx.enemy->GetWorldCoord() + glm::vec2(0,-10.0f);
		if (auto attackComp = enemy->GetComponent<AttackComponent>(ComponentType::ATTACK))
		{
			const auto weapon = attackComp->GetCurrentWeapon();
			if (const auto followComp = weapon->GetComponent<FollowerComponent>(ComponentType::FOLLOWER))
			{
				followComp->SetFollower(enemy);
				followComp->Update();
			}
		}
		if (const auto currentRoom = currentScene->GetCurrentRoom())
		{
			if (const auto monsterRoom = std::dynamic_pointer_cast<MonsterRoom>(currentRoom))
			{
				monsterRoom->AddEnemy(enemy);
			}
			if (const auto collisionManager = currentRoom->GetManager<RoomCollisionManager>(ManagerTypes::ROOMCOLLISION))
			{
				if (const auto collComp = enemy->GetComponent<CollisionComponent>(ComponentType::COLLISION)) {
					collisionManager->RegisterNGameObject(enemy);
					if (const std::shared_ptr<nGameObject>& colliderVisible = collComp->GetVisibleBox())
					{
						currentScene->GetRoot().lock()->AddChild(colliderVisible);
						currentScene->GetCamera().lock()->SafeAddChild(colliderVisible);
					}
				}
			}
			if (const auto trackingManager = currentRoom->GetManager<TrackingManager>(ManagerTypes::TRACKING))
			{
				LOG_DEBUG("add trackingManager");
				trackingManager->AddEnemy(std::dynamic_pointer_cast<Character>(enemy));
			}
		}
	}
	// if (const auto currentScene = SceneManager::GetInstance().GetCurrentScene().lock())
	// {
	// 	if (const auto currentRoom = currentScene->GetCurrentRoom())
	// 	{
	// 		if (const auto monsterRoom = std::dynamic_pointer_cast<MonsterRoom>(currentRoom))
	// 		{
	// 			auto pos = ctx.enemy->GetWorldCoord() + glm::vec2(0,-10.0f);
	// 			monsterRoom->SpawnEnemy(21, pos);
	// 		}
	// 	}
	// }
}