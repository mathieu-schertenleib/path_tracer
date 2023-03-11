add_library(imgui STATIC
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_draw.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_tables.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/imgui_widgets.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_glfw.cpp
        ${CMAKE_SOURCE_DIR}/external/imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui PRIVATE
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/external/glfw/include)


add_executable(path_tracer
        main.cpp
        render.cpp
        geometry.cpp)

target_include_directories(path_tracer PRIVATE
        ${CMAKE_SOURCE_DIR}/external/imgui
        ${CMAKE_SOURCE_DIR}/external/imgui/backends
        ${CMAKE_SOURCE_DIR}/external/stb)

target_compile_features(path_tracer PRIVATE cxx_std_20)

find_package(OpenGL REQUIRED)
target_link_libraries(path_tracer imgui glfw OpenGL::GL)

set(CLANG_OPTIONS
        -march=native
        -ffast-math
        -Wfatal-errors
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        #-Wold-style-cast
        -Wcast-align
        -Wunused
        -Woverloaded-virtual
        -Wpedantic
        -Wconversion
        -Wsign-conversion
        -Wnull-dereference
        -Wdouble-promotion)

set(GCC_OPTIONS
        ${CLANG_OPTIONS}
        -Wmisleading-indentation
        -Wduplicated-cond
        -Wduplicated-branches
        -Wlogical-op)

if (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    target_compile_options(path_tracer PRIVATE ${CLANG_OPTIONS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(path_tracer PRIVATE ${GCC_OPTIONS})
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(path_tracer PRIVATE /W4 /Zc:__cplusplus)
else ()
    message(WARNING "No compile options set for compiler '${CMAKE_CXX_COMPILER_ID}'")
endif ()

target_compile_definitions(path_tracer PRIVATE $<$<OR:$<CONFIG:Release>,$<CONFIG:MinSizeRel>>:NDEBUG>)
