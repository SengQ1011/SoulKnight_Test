//
// Created by tjx20 on 5/15/2025.
//

#ifndef IMAGEPOOLMANAGER_HPP
#define IMAGEPOOLMANAGER_HPP

#include "Core/Drawable.hpp"

namespace Util
{
	class Image;
	class Animation;
	class Text;
	class Color;
}

class ImagePoolManager {
public:
	// 獲取單一實例
	static ImagePoolManager& GetInstance();

	// 獲取Image資源
	std::shared_ptr<Core::Drawable> GetImage(const std::string& filepath);

	// 獲取Animation資源 --->不可行
	// std::shared_ptr<Core::Drawable> GetAnimation(const std::vector<std::string>& animationPaths, float interval, bool loop);

	// 獲取Text資源
	std::shared_ptr<Core::Drawable> GetText(const std::string& font, int size, const std::string& text, Util::Color color, bool useAA = true);

private:
	ImagePoolManager() = default;
	~ImagePoolManager() = default;

	// 禁止拷貝與賦值
	ImagePoolManager(const ImagePoolManager&) = delete;
	ImagePoolManager& operator=(const ImagePoolManager&) = delete;

	// 圖像池
	std::unordered_map<std::string, std::shared_ptr<Util::Image>> m_imagePool;

	// 動畫池：key (路徑+循環狀態 ==> 字串) --> 不可共用
	// std::unordered_map<std::string, std::shared_ptr<Util::Animation>> m_animationPool;

	// 文字池：key (字體+大小+文字 ==> 字串)
	std::unordered_map<std::string, std::shared_ptr<Util::Text>> m_textPool;

	// 生成文字資源的唯一 key
	std::string GenerateTextKey(const std::string& font, int size, const std::string& text, Util::Color color);
};

#endif //IMAGEPOOLMANAGER_HPP
