cmake_minimum_required(VERSION 3.5)
project(Framework VERSION 1.0.0 LANGUAGES CXX)

file(GLOB SOURCES fw/*.cpp fw/*.h)

file(GLOB IMGUI
    fw/imgui/*.cpp 
    fw/imgui/*.h)

add_library(Framework STATIC
    ${SOURCES}
    ${IMGUI}
)

target_compile_features(Framework 
    PUBLIC 
        cxx_std_11
)

target_compile_options(Framework PRIVATE /W3 /WX /MP)

target_link_libraries(Framework assimp DirectXTK d3d11 d3dcompiler)
target_include_directories(Framework PUBLIC ${PROJECT_SOURCE_DIR}/../External/assimp/include/)
target_compile_definitions(Framework PUBLIC -D_CRT_SECURE_NO_WARNINGS)