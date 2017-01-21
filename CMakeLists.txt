cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 11)

find_tml_package(tml)
find_tml_package(minecraft)
include_directories(${TML_INCLUDE_DIRS} ${MINECRAFT_INCLUDE_DIRS})
include_directories(include/)

add_library(guilib SHARED
        src/main.cpp include/guilib/element.h include/guilib/container.h include/guilib/window.h
        include/guilib/ninepatch.h include/guilib/button.h include/guilib/text.h include/guilib/color.h src/color.cpp src/element.cpp include/guilib/style_props_element.h include/guilib/style_props_text.h include/guilib/style_def.h include/guilib/style_common.h include/guilib/style_props_common.h)
target_link_libraries(guilib ${TML_LIBRARIES} ${MINECRAFT_LIBRARIES})
