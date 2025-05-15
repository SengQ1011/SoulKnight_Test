//
// Created by tjx20 on 5/15/2025.
//

#include "ImagePoolManager.hpp"
#include "Util/Animation.hpp"
#include "Util/Image.hpp"
#include "Util/Logger.hpp"
#include "Util/Text.hpp"

ImagePoolManager& ImagePoolManager::GetInstance() {
	static ImagePoolManager instance;
	return instance;
}

std::shared_ptr<Core::Drawable> ImagePoolManager::GetImage(const std::string& filepath) {
	if (m_imagePool.find(filepath) != m_imagePool.end()) {
		return m_imagePool[filepath];
	}
	// 如果找不到圖片資源，就創建並儲存
	auto drawable = std::make_shared<Util::Image>(filepath, false);
	m_imagePool[filepath] = drawable;
	return drawable;
}

// std::shared_ptr<Core::Drawable> ImagePoolManager::GetAnimation(const std::vector<std::string>& animationPaths, float interval, bool loop) {
// 	std::string key = GenerateAnimationKey(animationPaths, loop);
// 	if (m_animationPool.find(key) != m_animationPool.end()) {
// 		return m_animationPool[key];
// 	}
// 	// 如果找不到動畫資源，就創建並儲存
// 	auto animation = std::make_shared<Util::Animation>(animationPaths, false, interval, loop, 0);
// 	m_animationPool[key] = animation;
// 	return animation;
// }

std::shared_ptr<Core::Drawable> ImagePoolManager::GetText(const std::string& font, int size, const std::string& text, Util::Color color, bool useAA) {
	std::string key = GenerateTextKey(font, size, text, color);
	if (m_textPool.find(key) != m_textPool.end()) {
		return m_textPool[key];
	}
	// 如果找不到文字資源，就創建並儲存
	auto textDrawable = std::make_shared<Util::Text>(font, size, text, color, useAA);
	m_textPool[key] = textDrawable;
	return textDrawable;
}

// std::string ImagePoolManager::GenerateAnimationKey(const std::vector<std::string>& animationPaths, bool loop) {
// 	std::string key = "animation:";
// 	for (const auto& path : animationPaths) {
// 		key += path + ";";
// 	}
// 	key += loop ? "loop" : "no_loop";
// 	return key;
// }

std::string ImagePoolManager::GenerateTextKey(const std::string& font, int size, const std::string& text, Util::Color color) {
	return font + ":" + std::to_string(size) + ":" + text + ":" + color.ToString();
}
