set(SRC_FILES
    App.cpp
    Beacon.cpp
    Camera.cpp
    Cursor.cpp

    Weapon/Weapon.cpp
    Weapon/WeaponFactory.cpp

    Scene/SceneManager.cpp
    Scene/Test_Scene_KC.cpp
    Scene/Test_Scene_JX.cpp
    Scene/MainMenu_Scene.cpp
    Scene/Lobby_Scene.cpp
    Scene/DungeonLoading_Scene.cpp
    Scene/Dungeon_Scene.cpp
    Scene/Complete_Scene.cpp
    Scene/Result_Scene.cpp

    Room/Room.cpp

    RoomObject/ObstacleObject.cpp

    Creature/Character.cpp
    Creature/CharacterFactory.cpp
    Creature/Enemy.cpp
    Creature/Player.cpp

    Tool/CollisionBox.cpp
    Tool/HitBox.cpp
    Tool/Tool.cpp

)

set(INCLUDE_FILES
    Override/nGameObject.hpp

    Scene/SceneManager.hpp
    Scene/Scene.hpp
    Scene/Test_Scene_KC.hpp
    Scene/Test_Scene_JX.hpp
    Scene/MainMenu_Scene.hpp
    Scene/Lobby_Scene.hpp
    Scene/DungeonLoading_Scene.hpp
    Scene/Dungeon_Scene.hpp
    Scene/Complete_Scene.hpp
    Scene/Result_Scene.hpp

    Room/Room.hpp

    RoomObject/RoomObject.hpp
    RoomObject/ObstacleObject.hpp

    Weapon/Weapon.hpp
    Weapon/WeaponFactory.hpp

    Creature/Character.hpp
    Creature/CharacterFactory.hpp
    Creature/Enemy.hpp
    Creature/Player.hpp

    GameMechanism/Talent.hpp
    GameMechanism/Skill.hpp
    GameMechanism/StatusEffect.hpp

    Tool/CollisionBox.hpp
    Tool/HitBox.hpp
    Tool/Tool.hpp

    App.hpp
    Beacon.hpp
    Camera.hpp
    Cursor.hpp
    Animation.hpp
    Motion.hpp
)

set(TEST_FILES
)
