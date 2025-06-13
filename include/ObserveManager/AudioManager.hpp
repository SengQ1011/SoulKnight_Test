//
// Created by QuzzS on 2025/5/1.
//

#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <memory>
#include <string>
#include <unordered_map>


namespace Util
{
	class SFX;
	class BGM;
} // namespace Util

// TODO:如果音量引發性能問題RefreshAllVolum還可以優化
class AudioManager
{
public:
	static AudioManager &GetInstance();

	// 播放
	void PlaySFX(const std::string &name); // e.g., "enemy_die"
	// void PlayBGM(bool loop = true);
	void PlayBGM(const std::string &name, bool loop = true); // 播放指定BGM
	void FadeInSFX(const std::string &name, unsigned int tick, int loop = -1, unsigned int duration = -1);
	void FadeInBGM(int tick, int loop = -1);
	void FadeOutBGM(int tick);
	void PauseBGM();
	void ResumeBGM();

	// 音量
	void SetMasterVolume(float volume); // 0.0~1.0
	void SetSFXVolume(float volume); // 同上
	void SetBGMVolume(float volume);
	float GetMasterVolume() const { return m_MasterVolume; }
	float GetSFXVolume() const { return m_SFXVolume; }
	float GetBGMVolume() const { return m_BGMVolume; }

	void RegisterSFX(const std::string &name, const std::string &path);
	void RegisterBGM(const std::string &path);
	void RegisterBGM(const std::string &name, const std::string &path); // 註冊命名BGM

	void LoadFromJson(const std::string &path);
	void Reset(); // 清空SFX和BGM

	void Mute(bool mute);

	void DrawDebugUI();

private:
	void RefreshAllVolumes();
	std::unordered_map<std::string, std::shared_ptr<Util::SFX>> m_SFXs;
	std::shared_ptr<Util::BGM> m_BGM;
	std::unordered_map<std::string, std::shared_ptr<Util::BGM>> m_BGMs; // 多個BGM支援

	// 0%~100%
	float m_MasterVolume = 0.3f;
	float m_SFXVolume = 0.3f;
	float m_BGMVolume = 0.3f;
	bool m_Muted = false;

	AudioManager() = default;
};

#endif // AUDIOMANAGER_HPP
