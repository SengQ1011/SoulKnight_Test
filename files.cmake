set(SRC_FILES
    App.cpp
    Beacon.cpp
    Camera.cpp
    Cursor.cpp

    Scene/SceneManager.cpp
    Scene/Test_Scene_KC.cpp
    Scene/Test_Scene_JX.cpp
    Scene/MainMenu_Scene.cpp
    Scene/Lobby_Scene.cpp
    Scene/DungeonLoading_Scene.cpp
    Scene/Dungeon_Scene.cpp
    Scene/Complete_Scene.cpp
    Scene/Result_Scene.cpp

    Creature/Character.cpp
    Creature/Enemy.cpp
    Creature/Player.cpp

    Tool.cpp
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

    Weapon/Weapon.hpp

    Creature/Character.hpp
    Creature/Enemy.hpp
    Creature/Player.hpp

    GameMechanism/Talent.hpp
    GameMechanism/Skill.hpp
    GameMechanism/StatusEffect.hpp

    App.hpp
    Beacon.hpp
    Camera.hpp
    Cursor.hpp

    Motion.hpp
    Tool.hpp
)

set(TEST_FILES
)
