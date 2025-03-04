set(SRC_FILES
    App.cpp
    Beacon.cpp
    Camera.cpp
    Cursor.cpp

    Scene/Test_Scene.cpp
    Scene/MainMenu_Scene.cpp
    Scene/Game_Scene.cpp
    Scene/Lobby_Scene.cpp

    Creature/Character.cpp
    Creature/Player.cpp

    Button_UI.cpp
    TextObject_UI.cpp

    Tool.cpp
)

set(INCLUDE_FILES
    Override/nGameObject.hpp
    Override/nContext.hpp

    Scene/Scene.hpp
    Scene/Test_Scene.hpp
    Scene/MainMenu_Scene.hpp
    Scene/Game_Scene.hpp
    Scene/Lobby_Scene.hpp

    Weapon/Weapon.hpp

    Creature/Character.hpp
    Creature/Player.hpp

    GameMechanism/Talent.hpp
    GameMechanism/Skill.hpp
    GameMechanism/StatusEffect.hpp

    App.hpp
    Beacon.hpp
    Camera.hpp
    Cursor.hpp

    Button_UI.hpp
    TextObject_UI.hpp

    Motion.hpp
    Tool.hpp
)

set(TEST_FILES
)
