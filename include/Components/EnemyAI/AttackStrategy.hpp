//
// Created by tjx20 on 4/17/2025.
//

#ifndef ATTACKSTRATEGY_HPP
#define ATTACKSTRATEGY_HPP

#include <glm/vec2.hpp>
#include <unordered_map>
class Character;
struct EnemyContext;

//--------------------------------------------
// Strategy Interfaces
//--------------------------------------------
class IAttackStrategy {
public:
	virtual ~IAttackStrategy() = default;
	virtual void Update(const EnemyContext &ctx, const float deltaTime) = 0;

	[[nodiscard]] virtual bool CanAttack(const EnemyContext &ctx) = 0;
	[[nodiscard]] virtual float GetAttackDistance() const = 0;

protected:
	float m_attackCooldown = 5.0f;
	float m_attackTimer = 0.0f;
};

//--------------------------------------------
// Strategy Implementations
//--------------------------------------------
class MeleeAttack final : public IAttackStrategy
{
public:
	void Update(const EnemyContext &ctx, float deltaTime) override;
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) override;
	[[nodiscard]] float GetAttackDistance() const override { return m_meleeAttackDistance; }

private:
	const float m_meleeAttackDistance = 50.0f;
};

class GunAttack final : public IAttackStrategy
{
public:
	void Update(const EnemyContext &ctx, float deltaTime) override;
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) override;
	[[nodiscard]] float GetAttackDistance() const override { return m_gunAttackDistance; }

private:
	const float m_gunAttackDistance = 200.0f;
};

class CollisionAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override{}
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) override;
	[[nodiscard]] float GetAttackDistance() const override { return 0; }
};

class NoAttack final : public IAttackStrategy {
public:
	void Update(const EnemyContext &ctx, const float deltaTime) override {}
	[[nodiscard]] bool CanAttack(const EnemyContext &ctx) override{ return false; }
	[[nodiscard]] float GetAttackDistance() const override { return 0; }
};

//--------------------------------------------
// Boss
//--------------------------------------------
class BossAttackStrategy : public IAttackStrategy {
public:
    enum class BossSkillType {
        SKILL1,  // 雪球投掷
        SKILL2,  // 雪花风暴
        SKILL3,  // 火眼
        SKILL4,  // 冰霜荆棘
        SKILL5   // 雪人召唤
    };

    struct SkillInfo {
        float cooldown;        // 技能冷却时间
        float duration;        // 技能持续时间
        float castTime;        // 技能施放时间（前摇）
        bool isActive;         // 技能是否正在释放
        float activeTimer;     // 技能释放计时器
        int currentPhase;      // 技能当前阶段（用于多阶段技能）
    };

    BossAttackStrategy() {
        // 初始化技能信息
        skills[BossSkillType::SKILL1] = {5.0f, 6.0f, 0.0f, false, 0.0f, 0};  // 雪球投掷：5秒冷却，6秒持续，沒有前摇，共3 輪（2s/1phase）
        skills[BossSkillType::SKILL2] = {8.0f, 2.66, 0.055f, false, 0.0f, 0};  // 雪花风暴：8秒冷却，2秒持续，0.016秒前摇
        skills[BossSkillType::SKILL3] = {6.0f, 1.0f, 0.8f, false, 0.0f, 0};  // 火眼：6秒冷却，1秒持续，0.8秒前摇
        skills[BossSkillType::SKILL4] = {7.0f, 1.29f, 1.0f, false, 0.0f, 0};  // 冰霜荆棘：7秒冷却，1.5秒持续，0.4秒前摇
        skills[BossSkillType::SKILL5] = {12.0f, 2.9166f, 0.5f, false, 0.0f, 0}; // 雪人召唤：12秒冷却，4秒持续，0.2秒前摇
    }

    [[nodiscard]] bool CanAttack(const EnemyContext &ctx) override;
    [[nodiscard]] float GetAttackDistance() const override { return 100.0F; }
    void Update(const EnemyContext& ctx, const float deltaTime) override;

    // 获取当前技能信息
    [[nodiscard]] const SkillInfo& GetCurrentSkillInfo() const { return skills.at(currentSkill); }
    [[nodiscard]] bool IsSkillActive() const { return skills.at(currentSkill).isActive; }
    [[nodiscard]] float GetSkillRemainingTime() const { return skills.at(currentSkill).activeTimer; }

private:
    BossSkillType currentSkill = BossSkillType::SKILL1;
    std::unordered_map<BossSkillType, SkillInfo> skills;

    void ExecuteCurrentSkill(const EnemyContext& ctx);

    // Skill
    void ExecuteSkill1(const EnemyContext& ctx);  // 雪球投掷
    void ExecuteSkill2(const EnemyContext& ctx);  // 雪花风暴
    void ExecuteSkill3(const EnemyContext& ctx);  // 火眼
    void ExecuteSkill4(const EnemyContext& ctx);  // 冰霜荆棘
    void ExecuteSkill5(const EnemyContext& ctx);  // 雪人召唤

    void SpawnLargeSnowball(const EnemyContext& ctx, const glm::vec2& direction);
    void SpawnSmallSnowball(const EnemyContext& ctx, const glm::vec2& direction, const glm::vec2& offset);
    void SpawnLongBullet(const EnemyContext& ctx, const glm::vec2& direction);
    void SpawnIceSpike(const EnemyContext& ctx, const glm::vec2& direction);
    void SpawnShockwave(const EnemyContext& ctx);
    void SpawnMiniSnowman(const EnemyContext& ctx, const glm::vec2& position);
};
#endif //ATTACKSTRATEGY_HPP