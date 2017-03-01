cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 11)

include_directories(include/)
include_directories(example/src/)

find_package(PNG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PkgConfig)

find_path(GLFW_INCLUDE_DIRS GLFW/glfw3.h)
find_library(GLFW_LIBRARIES NAMES GLFW glfw3 glfw PATH_SUFFIXES lib64)
IF (NOT GLFW_LIBRARIES)
    pkg_search_module(GLFW REQUIRED glfw3)
ENDIF()

find_path(EPOXY_INCLUDE_DIRS epoxy/gl.h)
find_library(EPOXY_LIBRARIES NAMES GLFW epoxy PATH_SUFFIXES lib64)
IF (NOT EPOXY_LIBRARIES)
    pkg_search_module(GLFW REQUIRED epoxy)
ENDIF()

find_path(UTF8PROC_INCLUDE_DIRS utf8proc.h)
find_library(UTF8PROC_LIBRARIES NAMES utf8proc PATH_SUFFIXES lib64)
IF (NOT UTF8PROC_LIBRARIES)
    pkg_search_module(UTF8PROC REQUIRED utf8proc)
ENDIF()

include_directories(${OPENGL_INCLUDE_DIRS} ${EPOXY_INCLUDE_DIRS} ${GLFW_INCLUDE_DIRS} ${UTF8PROC_INCLUDE_DIRS})

add_executable(guilib
        src/main.cpp
        include/guilib/element.h include/guilib/container.h include/guilib/window.h include/guilib/ninepatch.h
        include/guilib/button.h include/guilib/text.h include/guilib/color.h include/guilib/style_props_text.h
        include/guilib/style_def.h include/guilib/style_common.h include/guilib/style_props_common.h
        include/guilib/style_def_callbacks.h include/guilib/display_mode.h include/guilib/style_props_element.h
        include/guilib/inline_builder.h include/guilib/mesh.h include/guilib/mesh_builder.h include/guilib/font.h
        src/color.cpp src/element.cpp src/container.cpp src/text.cpp src/inline_builder.cpp src/font.cpp
        include/guilib/style_props_container.h include/guilib/layout_properties.h include/guilib/css_tokenizer.h
        src/css_tokenizer.cpp src/string_util.cpp

        example/src/mesh_impl.h example/src/mesh_builder_impl.h example/src/texture_impl.h example/src/shader.h
        example/src/image.h example/src/ascii_pixel_font.h
        example/src/window.cpp example/src/ascii_pixel_font.cpp example/src/mesh_impl.cpp example/src/texture_impl.cpp
        example/src/mesh_builder_impl.cpp example/src/shader.cpp example/src/image.cpp)
target_link_libraries(guilib ${OPENGL_LIBRARIES} ${PNG_LIBRARIES} ${GLFW_LIBRARIES} ${EPOXY_LIBRARIES}
        ${UTF8PROC_LIBRARIES})
