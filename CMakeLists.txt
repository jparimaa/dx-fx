cmake_minimum_required(VERSION 3.5)
project(dx-fx VERSION 0.0.1 LANGUAGES CXX)

if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message(FATAL_ERROR "64bit is required")
endif()

add_definitions(
    -DASSETS_PATH="${CMAKE_CURRENT_SOURCE_DIR}/Assets/"
    -DROOT_PATH="${CMAKE_CURRENT_SOURCE_DIR}/"
    -DNOMINMAX
)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) 

add_subdirectory(External/assimp)
add_subdirectory(External/)

function(ADD_PROJECT_WITH_DEFAULT_SETTINGS PROJECT_NAME)
    file(GLOB SOURCES 
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp 
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES FOLDER "Examples")

    target_link_libraries(${PROJECT_NAME}
        PRIVATE
            Framework
    )

    target_include_directories(${PROJECT_NAME} PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/../../Framework/"
    )

    target_compile_options(${PROJECT_NAME} PRIVATE /W3 /WX /MP)
    target_compile_definitions(${PROJECT_NAME} PUBLIC -D_CRT_SECURE_NO_WARNINGS)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_BINARY_DIR}/External/assimp/code/debug/assimp-vc140-mt.dll"
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endfunction()

add_subdirectory(Framework)
add_subdirectory(Examples/Minimal)
add_subdirectory(Examples/CSM)
add_subdirectory(Examples/Multithread)
add_subdirectory(Examples/BrokenGlass)
add_subdirectory(Examples/Imgui)
add_subdirectory(Examples/Fullscreen)
add_subdirectory(Examples/TAA)
add_subdirectory(Examples/SDF)
add_subdirectory(Examples/VolumetricExplosion)