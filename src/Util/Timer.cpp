//
// Created by QuzzS on 2025/6/5.
//

#include "Util/Timer.hpp"
#include <algorithm>
#include "Util/Time.hpp"

namespace Util
{
	Timer::Timer(float duration) : m_Duration(duration) {}

	void Timer::Start() { m_IsRunning = true; }

	void Timer::Stop() { m_IsRunning = false; }

	void Timer::Reset()
	{
		m_ElapsedTime = 0.0f;
		m_IsRunning = false;
		m_HasFinishedLastFrame = false;
	}

	void Timer::Update()
	{
		if (m_IsRunning)
		{
			m_ElapsedTime += Util::Time::GetDeltaTimeMs() / 1000.0f;

			// 調用更新回調函數
			if (m_OnUpdate)
			{
				const float progress = GetProgressClamped();
				m_OnUpdate(progress);
			}

			// 檢查是否完成
			const bool isFinished = m_ElapsedTime >= m_Duration;

			// 如果剛完成且有完成回調，調用它
			if (isFinished && !m_HasFinishedLastFrame && m_OnFinished)
			{
				m_OnFinished();
			}

			// 更新狀態
			m_HasFinishedLastFrame = isFinished;

			// 如果時間到了，自動停止
			if (isFinished)
			{
				m_IsRunning = false;
			}
		}
	}

	void Timer::SetDuration(float duration) { m_Duration = duration; }

	float Timer::GetDuration() const { return m_Duration; }

	float Timer::GetElapsedTime() const { return m_ElapsedTime; }

	float Timer::GetProgress() const
	{
		if (m_Duration <= 0.0f)
			return 1.0f;

		return m_ElapsedTime / m_Duration;
	}

	float Timer::GetProgressClamped() const { return std::min(GetProgress(), 1.0f); }

	bool Timer::IsRunning() const { return m_IsRunning; }

	bool Timer::IsFinished() const { return m_ElapsedTime >= m_Duration; }

	void Timer::Restart()
	{
		Reset();
		Start();
	}

	void Timer::SetAndStart(float duration)
	{
		SetDuration(duration);
		Restart();
	}

	void Timer::SetOnFinished(std::function<void()> callback) { m_OnFinished = callback; }

	void Timer::SetOnUpdate(std::function<void(float)> callback) { m_OnUpdate = callback; }

	void Timer::ClearCallbacks()
	{
		m_OnFinished = nullptr;
		m_OnUpdate = nullptr;
	}

	float Timer::GetEasedProgress(float (*easingFunction)(float)) const
	{
		const float progress = GetProgressClamped();
		return easingFunction ? easingFunction(progress) : progress;
	}

	// 緩動函數實現
	float Timer::EaseLinear(float t) { return t; }

	float Timer::EaseOutQuad(float t) { return 1.0f - (1.0f - t) * (1.0f - t); }

	float Timer::EaseInQuad(float t) { return t * t; }

	float Timer::EaseInOutQuad(float t)
	{
		if (t < 0.5f)
			return 2.0f * t * t;
		else
			return 1.0f - 2.0f * (1.0f - t) * (1.0f - t);
	}
} // namespace Util
