//
// Created by QuzzS on 2025/6/5.
//

#ifndef TIMER_HPP
#define TIMER_HPP

#include <functional>

namespace Util
{
	/**
	 * @brief 計時器類別，用於動畫和時間相關的功能
	 */
	class Timer
	{
	public:
		Timer() = default;
		explicit Timer(float duration);
		~Timer() = default;

		// 基本操作
		void Start();
		void Stop();
		void Reset();
		void Update();

		// 設定和取得
		void SetDuration(float duration);
		float GetDuration() const;
		float GetElapsedTime() const;
		float GetProgress() const; // 返回 0.0f 到 1.0f 的進度
		float GetProgressClamped() const; // 確保不超過 1.0f

		// 狀態查詢
		bool IsRunning() const;
		bool IsFinished() const;

		// 便利方法
		void Restart(); // 重置並開始
		void SetAndStart(float duration); // 設定時間並開始

		// Callback 功能
		void SetOnFinished(std::function<void()> callback); // 設定完成時的回調函數
		void SetOnUpdate(std::function<void(float)> callback); // 設定每幀更新的回調函數
		void ClearCallbacks(); // 清除所有回調函數

		// 緩動函數支援
		float GetEasedProgress(float (*easingFunction)(float)) const;

		// 常用的緩動函數
		static float EaseLinear(float t);
		static float EaseOutQuad(float t);
		static float EaseInQuad(float t);
		static float EaseInOutQuad(float t);

	private:
		float m_Duration = 1.0f; // 計時器持續時間
		float m_ElapsedTime = 0.0f; // 已經過的時間
		bool m_IsRunning = false; // 是否正在運行
		bool m_HasFinishedLastFrame = false; // 上一幀是否已完成，用於避免重複回調

		// Callback 函數
		std::function<void()> m_OnFinished; // 完成時的回調
		std::function<void(float)> m_OnUpdate; // 每幀更新的回調(參數為當前進度)
	};
} // namespace Util

#endif // TIMER_HPP
