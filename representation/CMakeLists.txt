add_executable(bomberman
        src/main.cpp
        src/Game.cpp
        src/ConcreteFactory.cpp
        src/MenuState.cpp
        src/PlayingState.cpp
        src/GameOverState.cpp
)

target_include_directories(bomberman PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.1
)

FetchContent_MakeAvailable(SFML)

add_custom_command(TARGET bomberman POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:bomberman>/assets
)

if (WIN32)
    add_custom_command(TARGET bomberman POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll"
            $<TARGET_FILE_DIR:bomberman>
    )

    add_custom_command(TARGET bomberman POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-src/extlibs/bin/x64/openal32.dll"
            "${CMAKE_BINARY_DIR}/representation"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-system$<$<CONFIG:Debug>:-d>-2.dll"
            "${CMAKE_BINARY_DIR}/representation"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-window$<$<CONFIG:Debug>:-d>-2.dll"
            "${CMAKE_BINARY_DIR}/representation"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-graphics$<$<CONFIG:Debug>:-d>-2.dll"
            "${CMAKE_BINARY_DIR}/representation"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_BINARY_DIR}/_deps/sfml-build/lib/sfml-audio$<$<CONFIG:Debug>:-d>-2.dll"
            "${CMAKE_BINARY_DIR}/representation"
    )
endif ()

target_link_libraries(bomberman PRIVATE
        bomberman_logic
        sfml-graphics
        sfml-window
        sfml-system
        sfml-audio
)

target_compile_features(bomberman PUBLIC cxx_std_17)

if (MSVC)
    target_compile_options(bomberman PRIVATE /W4)
else ()
    target_compile_options(bomberman PRIVATE -Wall -Wextra -Wpedantic)
endif ()

# Copy the assets folder next to the built binary so relative paths like
# "assets/player.png" (see ConcreteFactory) resolve correctly.
add_custom_command(TARGET bomberman POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:bomberman>/assets
)
