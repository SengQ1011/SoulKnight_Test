cmake_minimum_required(VERSION 3.16)
project(SoulKnight)

set(CMAKE_CXX_STANDARD 17)

cmake_policy(SET CMP0135 NEW)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

FetchContent_Declare(
    ptsd

    URL         https://github.com/ntut-open-source-club/practical-tools-for-simple-design/archive/refs/tags/v0.2.zip

    # Or alternatively if you want to use a specific git commit instead of a release:
    # GIT_REPOSITORY  https://github.com/ntut-open-source-club/practical-tools-for-simple-design.git
    # GIT_TAG         <git_tag>

    SOURCE_DIR  ${CMAKE_CURRENT_SOURCE_DIR}/PTSD
)
FetchContent_MakeAvailable(ptsd)

FetchContent_Declare(
        eventpp
        GIT_REPOSITORY https://github.com/wqking/eventpp
        GIT_TAG        d2db8af2a46c79f8dc75759019fba487948e9442 # v0.1.3
        SOURCE_DIR     ${CMAKE_CURRENT_SOURCE_DIR}/EVENTPP
)
FetchContent_MakeAvailable(eventpp)

include(files.cmake)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
list(TRANSFORM SRC_FILES PREPEND ${SRC_DIR}/)
list(TRANSFORM INCLUDE_FILES PREPEND ${INCLUDE_DIR}/)
list(TRANSFORM INLINE_FILES PREPEND ${INCLUDE_DIR}/)

#file(GLOB_RECURSE SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
#file(GLOB_RECURSE INCLUDE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp)

add_executable(${PROJECT_NAME}
    ${SRC_DIR}/main.cpp
    ${SRC_FILES}
    ${INCLUDE_FILES}
    ${INLINE_FILES}
)

if(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE /W4)
else()
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -pedantic)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Resources")
<<<<<<< Updated upstream
    target_compile_definitions(${PROJECT_NAME} PRIVATE JSON_DIR="${CMAKE_CURRENT_SOURCE_DIR}/json")
else()
    message(FATAL_ERROR "relative RESOURCE_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
    # target_compile_definitions(${PROJECT_NAME} PRIVATE RESOURCE_DIR="${CMAKE_INSTALL_PREFIX}/Resources")
endif()

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PTSD_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/PTSD/assets")
else()
    # TODO
    message(AUTHOR_WARNING "relative PTSD_ASSETS_DIR is WIP, Please use `-DCMAKE_BUILD_TYPE=Debug` build for now.")
    target_compile_definitions(${PROJECT_NAME} PRIVATE PTSD_ASSETS_DIR="${CMAKE_CURRENT_SOURCE_DIR}/PTSD/assets")
endif()

target_include_directories(${PROJECT_NAME} SYSTEM PRIVATE ${DEPENDENCY_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PTSD/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/EVENTPP/include)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/nlohmann)

target_link_libraries(${PROJECT_NAME}
    SDL2::SDL2main
    PTSD
    eventpp
)

# 定義生成 files.cmake 的函數
function(generate_files_cmake)
    # 搜索所有源文件和頭文件
    file(GLOB_RECURSE ALL_SRC_FILES "${CMAKE_SOURCE_DIR}/src/*.cpp")
    file(GLOB_RECURSE ALL_INCLUDE_FILES "${CMAKE_SOURCE_DIR}/include/*.hpp")
    file(GLOB_RECURSE ALL_INLINE_FILES "${CMAKE_SOURCE_DIR}/include/*.inl")

    # 獲取相對路徑
    set(REL_SRC_FILES "")
    foreach(FILE ${ALL_SRC_FILES})
        file(RELATIVE_PATH REL_FILE "${CMAKE_SOURCE_DIR}/src" ${FILE})
        list(APPEND REL_SRC_FILES ${REL_FILE})
    endforeach()

    set(REL_INCLUDE_FILES "")
    foreach(FILE ${ALL_INCLUDE_FILES})
        file(RELATIVE_PATH REL_FILE "${CMAKE_SOURCE_DIR}/include" ${FILE})
        list(APPEND REL_INCLUDE_FILES ${REL_FILE})
    endforeach()

    set(REL_INLINE_FILES "")
    foreach(FILE ${ALL_INLINE_FILES})
        file(RELATIVE_PATH REL_FILE "${CMAKE_SOURCE_DIR}/include" ${FILE})
        list(APPEND REL_INLINE_FILES ${REL_FILE})
    endforeach()

    # 排序文件列表以保持一致性
    list(SORT REL_SRC_FILES)
    list(SORT REL_INCLUDE_FILES)
    list(SORT REL_INLINE_FILES)

    # 生成新內容
    set(NEW_CONTENT "set(SRC_FILES\n")
    foreach(FILE ${REL_SRC_FILES})
        string(APPEND NEW_CONTENT "    ${FILE}\n")
    endforeach()
    string(APPEND NEW_CONTENT ")\n\nset(INCLUDE_FILES\n")
    foreach(FILE ${REL_INCLUDE_FILES})
        string(APPEND NEW_CONTENT "    ${FILE}\n")
    endforeach()
    string(APPEND NEW_CONTENT ")\n\nset(INLINE_FILES\n")
    foreach(FILE ${REL_INLINE_FILES})
        string(APPEND NEW_CONTENT "    ${FILE}\n")
    endforeach()
    string(APPEND NEW_CONTENT ")\n")

    # 檢查是否需要更新 files.cmake
    set(NEED_UPDATE TRUE)
    if(EXISTS "${CMAKE_SOURCE_DIR}/files.cmake")
        file(READ "${CMAKE_SOURCE_DIR}/files.cmake" OLD_CONTENT)

        # 比較新舊內容
        if("${OLD_CONTENT}" STREQUAL "${NEW_CONTENT}")
            set(NEED_UPDATE FALSE)
        endif()
    endif()

    # 如果需要，更新 files.cmake
    if(NEED_UPDATE)
        file(WRITE "${CMAKE_SOURCE_DIR}/files.cmake" ${NEW_CONTENT})
        message(STATUS "已更新 files.cmake")
    else()
        message(STATUS "files.cmake 未變更，無需更新")
    endif()

endfunction()

generate_files_cmake()
