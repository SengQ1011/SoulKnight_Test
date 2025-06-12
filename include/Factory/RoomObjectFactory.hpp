//
// Created by QuzzS on 2025/3/27.
//

#ifndef ROOMOBJECTFACTORY_HPP
#define ROOMOBJECTFACTORY_HPP

#include "EnumTypes.hpp"
#include "Factory.hpp"
#include "glm/glm.hpp"


enum class MineType;
enum class ShopItemType;
class DestructibleObject;
class Loader;
class nGameObject;
class Character;
enum class ChestType;

// 藥水相關枚舉已移至 EnumTypes.hpp

/// @brief:建造RoomObject前必須設置ScenePath(從Room取得)
class RoomObjectFactory : public Factory
{
public:
	explicit RoomObjectFactory(const std::shared_ptr<Loader> &loader) : m_Loader(loader) {}
	~RoomObjectFactory() override = default;

	// 根據配置文件創建物件
	std::shared_ptr<nGameObject> CreateRoomObject(const std::string &_id, const std::string &_class = "");

	// 地形創建專用方法
	std::shared_ptr<nGameObject> CreateWall(int row, int col, const glm::vec2 &worldPos);
	std::shared_ptr<nGameObject> CreateFloor(const glm::vec2 &worldPos);
	std::shared_ptr<nGameObject> CreateDoor(const glm::vec2 &worldPos);

	// 寶箱創建方法
	std::shared_ptr<nGameObject> CreateChest(ChestType type, const std::shared_ptr<Character> &player);

	// 商店桌子創建方法
	std::shared_ptr<nGameObject> CreateShopTable(ShopItemType itemType, int price,
												 const std::shared_ptr<Character> &player,
												 PotionSize potionSize = PotionSize::SMALL);

	std::shared_ptr<DestructibleObject> CreateMine(MineType type);

	// 藥水創建方法
	std::shared_ptr<nGameObject> CreatePotion(PotionType potionType, PotionSize potionSize = PotionSize::SMALL);

	// 掉落物品創建方法
	std::vector<std::shared_ptr<nGameObject>> CreateDropItems(const std::string &itemType, int quantity,
															  float scale = 1.0f);

	[[nodiscard]] std::string GetObjectDataFilePath() const { return m_ObjectDataFilePath; }
	void SetObjectDataFilePath(const std::string &prePath) { m_ObjectDataFilePath = prePath + "ObjectData/"; }

	// TODO:測試std::string m_ObjectDataFilePath; /// @param:"Scene(Theme)/ObjectData/"作爲_id的前綴
	std::string m_ObjectDataFilePath = JSON_DIR "/Lobby/ObjectData"; /// @param:"Scene(Theme)/ObjectData/"作爲_id的前綴
	std::weak_ptr<Loader> m_Loader;
};

#endif // ROOMOBJECTFACTORY_HPP
