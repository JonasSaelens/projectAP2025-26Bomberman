add_library(bomberman_logic STATIC
        src/PowerUp.cpp
        src/Bot.cpp
        src/Score.cpp
        src/World.cpp
)

target_include_directories(bomberman_logic PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(bomberman_logic PUBLIC cxx_std_17)

if (MSVC)
    target_compile_options(bomberman_logic PRIVATE /W4)
else ()
    target_compile_options(bomberman_logic PRIVATE -Wall -Wextra -Wpedantic)
endif ()
