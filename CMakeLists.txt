cmake_minimum_required(VERSION 3.10)
project(NotYourOwnAdventure)
set(CMAKE_CXX_STANDARD 17)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

add_executable(NotYourOwnAdventure
    main.cpp
    Game.cpp
)

target_link_libraries(NotYourOwnAdventure
    sfml-graphics
    sfml-window
    sfml-system
)