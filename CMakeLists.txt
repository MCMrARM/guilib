cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 11)

find_tml_package(tml)
find_tml_package(minecraft)
include_directories(${TML_INCLUDE_DIRS} ${MINECRAFT_INCLUDE_DIRS})
include_directories(include/)

add_library(guilib SHARED
        src/main.cpp include/guilib/skin.h include/guilib/element.h include/guilib/container.h include/guilib/window.h
        include/guilib/ninepatch.h include/guilib/button.h include/guilib/label.h include/guilib/color.h src/color.cpp src/element.cpp)
target_link_libraries(guilib ${TML_LIBRARIES} ${MINECRAFT_LIBRARIES})
